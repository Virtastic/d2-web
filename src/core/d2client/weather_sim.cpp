// D2Client weather driver, 1.13c. Ported from the disassembly (the decompiler loses the register arguments
// of 0x18D0 and the x87 math). The hero seed is *(D2Client+0x11BBFC)+0x20; every draw below advances it.
#include "weather_sim.h"

#include <cmath>
#include <cstddef>

float FOG_10083_Cos_LUT(int16_t i);   // Fog #10083 (0x6FABBFFE): 512-step circle
float FOG_10084_Sin_LUT(int16_t i);   // Fog #10084 (0x6FABBFE6)

namespace d2client::weather {
namespace {

constexpr uint32_t kMul = 0x6AC690C5u;

// One LCG step (the inline `mul 0x6AC690C5 / add hi / adc 0` blocks); returns the new low dword.
inline uint32_t step(Seed& s) {
    const uint64_t v = uint64_t(s.lo) * kMul + s.hi;
    s.lo = uint32_t(v);
    s.hi = uint32_t(v >> 32);
    return s.lo;
}

// D2Client+0x18D0, register args ESI = range, EDI = offset.
inline int32_t roll(Seed& s, int32_t range, int32_t offset) {
    if (range < 1) return offset;
    const uint32_t r = uint32_t(range);
    const uint32_t lo = step(s);
    return int32_t(((r & (r - 1)) == 0 ? (lo & (r - 1)) : (lo % r)) + uint32_t(offset));
}

inline int32_t ftol(double v) { return int32_t(v); }   // _ftol: truncation toward zero

// Pool at DAT_6FBCBF68: 0x100 slots of 0x28 bytes (Fog 10064/10065 allocator, same rules as
// monspawn/weather_pool.h: forward scan from a cursor that frees pull back).
struct Particle {
    uint16_t active;   // +0
    int32_t x;         // +4
    int32_t y;         // +8
    int32_t bound;     // +0xC  (y at which the drop lands)
    int32_t frame;     // +0x10
    int32_t speed;     // +0x14
    int32_t phase;     // +0x18
    int32_t landed;    // +0x1C
    int32_t splash;    // +0x20
};
constexpr int kCap = 0x100;

struct State {
    Particle pool[kCap];
    uint32_t cursor = 0;
    int32_t maxIndex = -1, live = 0;
    uint16_t gen = 0x8000;

    int32_t t3934 = 0, t3914 = 0, t3918 = 0, t394c = 0, t3938 = 0, t38bc = 0;
    bool t393c = false;
    float f38f4 = 0.0f;
    int32_t bf6c = 0, bf78 = 0, bf7c = 0, bf80 = 0, bf84 = 0, bf88 = 0;
    int32_t d3438 = 0, d343c = 0, d3440 = 0;
    int32_t tabA[4] = {0, 0, 0, 0}, tabB[4] = {0, 0, 0, 0};
    bool setup = false;
};
State g;

void poolClear() {
    for (Particle& p : g.pool) p = Particle{};
    g.cursor = 0; g.maxIndex = -1; g.live = 0; g.gen = 0x8000;
}
int32_t poolAlloc() {
    uint32_t i = g.cursor;
    while (i < kCap && g.pool[i].active) ++i;
    if (i >= kCap) return -1;
    g.pool[i] = Particle{};
    g.pool[i].active = g.gen++;
    if (g.gen == 0) g.gen = 0x8000;
    ++g.live;
    if (g.maxIndex < int32_t(i)) g.maxIndex = int32_t(i);
    g.cursor = i + 1;
    return int32_t(i);
}
void poolFree(int32_t i) {
    g.pool[i].active = 0;
    if (uint32_t(i) < g.cursor) g.cursor = uint32_t(i);
    if (i == g.maxIndex) while (g.maxIndex >= 0 && !g.pool[g.maxIndex].active) --g.maxIndex;
    --g.live;
}

// 1.13c D2Client+0x80700: spawn one particle (5 hero-seed draws).
void spawn(Seed& s, const Env& e) {
    const int32_t slot = poolAlloc();
    if (slot < 0) return;
    Particle& p = g.pool[slot];
    p.landed = 0;
    p.x = roll(s, e.cellW, 0);
    const int32_t range = e.cellH - 0x57;
    p.bound = roll(s, range, 0x28);
    p.y = roll(s, p.bound + 0x14, -0x14);
    p.phase = int32_t(step(s) & 0x1ff);
    const int32_t a = p.bound - 0x28;
    const float f = float(double(a) / double(range));
    if (g.bf78 != 0) {
        step(s);                                   // %100, unused here (native stores it to a dead slot)
        p.splash = 1;
        p.speed = 8 - ftol(double(f) * -28.0);
        p.frame = a * 7 / range + (g.bf7c ? 1 : 0);
    } else {
        step(s);                                   // %12: colour-table index
        p.frame = 4 - ftol(double(f) * -8.0);
        p.speed = 15 - ftol(double(f) * -15.0);
        p.splash = 3;
    }
}

// 1.13c D2Client+0x80BE0
void init80BE0(Seed& s) {
    g.t3938 = int32_t(step(s) % 0x177u) + 0x7d;
    g.t38bc = int32_t(step(s) % 0x5dcu) + 500;
    g.t3918 = int32_t(step(s) % 0x47u) + 0x5c;
    g.t394c = g.t3918;
}

// 1.13c D2Client+0x80CA0
void phaseSteady(Seed& s, const Env& e) {
    switch (e.levelId) {
    case 0x6d: case 0x6e: case 0x6f: case 0x70:
        g.t3914 = int32_t(step(s) % 0x38u) + 0x20; g.t3934 = g.t3914; return;
    case 0x75:
        g.t3914 = int32_t(step(s) % 0x70u) + 0x20; g.t3934 = g.t3914; return;
    case 0x78: case 0x79:
        g.t394c = g.t3918 = 0x1c; g.t3914 = g.t3934 = 0x100; return;
    }
    g.t3934 = g.t3914;
    if (g.bf78 != 0) { g.bf6c = 0; return; }
    step(s);
    g.bf6c = 0;
}

// 1.13c D2Client+0x81470 (ESI = new rain mode)
void setRainMode(int32_t mode, int32_t param) {
    if (g.bf6c != 0 && mode != 0) g.bf6c = 0;
    if (g.bf80 != 0 && mode == 0) g.bf80 = 0;
    if (mode != g.bf78 || param != 0) poolClear();   // Ordinal_10063
    g.bf78 = mode;
}

// 1.13c D2Client+0x814F0 (per-level setup)
void levelSetup(Seed& s, const Env& e) {
    if (e.act != 4) {
        g.bf80 = 0;
        if (g.bf78 != 0) poolClear();
        g.bf78 = 0;
        init80BE0(s);
        return;
    }
    init80BE0(s);
    int32_t threshold = 0, wet = 0;
    switch (e.levelId) {
    case 0x6d: case 0x6e: case 0x6f: case 0x70: g.bf7c = 0; threshold = 0x19; break;
    case 0x78: case 0x79: case 0x75:            g.bf7c = 1; wet = 1; break;
    }
    const int32_t r = int32_t(step(s) % 100u);
    if (r < threshold) {                           // `&& param == 0`: the native param is always 0 here
        g.bf80 = 0;
        if (g.bf78 != 0) poolClear();
        g.bf78 = 0;
        return;
    }
    g.bf80 = wet;                                   // 0x812F0
    if (wet != 0 && g.bf84 == 0) { g.bf88 = 2; phaseSteady(s, e); }
    setRainMode(1, 0);                              // 0x81470
}

// 1.13c D2Client+0x81120 (EAX = phase)
void enterPhase(Seed& s, const Env& e, int32_t ph) {
    if (ph < 0 || ph >= 4) return;
    if ((g.bf84 == 0 || ph == 0) && (g.bf80 == 0 || ph == 2)) {
        g.bf88 = ph;
        switch (ph) {
        case 0: g.t3934 = 0; break;
        case 1: levelSetup(s, e); g.t3914 = roll(s, 0xe0, 0x20); break;
        case 2: phaseSteady(s, e); break;
        case 3: g.bf6c = 0; break;
        }
    }
}

// 1.13c D2Client+0x811F0
void phaseCycle(Seed& s, const Env& e) {
    if (g.bf80 == 0 || g.bf88 == 2) {
        int32_t ph = g.bf88;
        if (g.d343c == 0) {
            ph = g.bf88 + 1;
            if (ph == 4) ph = 0;
            g.d3438 = roll(s, g.tabA[ph], g.tabB[ph]);
            g.d343c = g.d3438;
            enterPhase(s, e, ph);
        }
        --g.d343c;
        if (ph == 1)
            g.t3934 = int32_t(uint32_t(int64_t(g.d3438 - g.d343c) * g.t3914) / uint32_t(g.d3438));
        else if (ph == 3)
            g.t3934 = int32_t(uint32_t(g.t3914) * uint32_t(g.d343c) / uint32_t(g.d3438));
        g.f38f4 = float(double(g.t3934) * double(1.0f / 256.0f));
    } else if (g.bf84 == 0) {
        g.bf88 = 2;
        phaseSteady(s, e);
    }
}

// 1.13c D2Client+0x80F60
void update(Seed& s, const Env& e) {
    float scale;
    if (g.bf78 != 0) {
        double c = std::fabs(double(FOG_10083_Cos_LUT(int16_t(g.t3918))));
        scale = (c > 0.25) ? 0.25f : float(c);   // fcomp 0.25; jp skips the clamp when the compare is unordered/greater
        if (!(c > 0.25)) scale = float(c);
        // Native: after `fabs; fst [s]; fcomp 0.25; test ah,5; jp` the clamp store happens only when
        // C0|C2 are both clear, i.e. |c| > 0.25. Otherwise |c| stays.
        scale = (c > 0.25) ? 0.25f : float(c);
    } else {
        scale = float(double(g.f38f4) * double(0.15f) + double(0.85f));
    }
    for (int32_t i = 0; i <= g.maxIndex; ++i) {
        Particle* p = &g.pool[i];
        if (!p->active) continue;
        const int32_t t = ftol(double(p->speed) * double(scale));
        const float v = float(t);
        int32_t dx = ftol(double(FOG_10083_Cos_LUT(int16_t(g.t3918))) * double(v));
        const int32_t dy = ftol(double(FOG_10084_Sin_LUT(int16_t(g.t3918))) * double(v));
        p->y = p->y + (dy - e.tickDy);
        if (p->y > p->bound) p->landed = 1;
        if (p->landed != 0) {
            if (p->splash == 0) {
                poolFree(i);
                if (g.live < g.t3934) spawn(s, e);
            } else {
                dx = 0;
                p->speed = 0;
                --p->splash;
            }
        }
        if (!p->active) continue;
        if (g.bf78 != 0 && p->landed == 0) {
            const uint32_t m = uint32_t(g.d3440) << 9;
            const uint32_t q = uint32_t((uint64_t(m) * 0x51eb851fu) >> 32) >> 3;
            const int32_t idx = int32_t((q + uint32_t(p->phase)) & 0x1ff);
            dx += ftol(double(FOG_10084_Sin_LUT(int16_t(idx))) * 2.0);
        }
        p->x = p->x - e.tickDx + dx;
        if (p->x >= e.cellW) p->x -= e.cellW;
        if (p->x < 0) p->x += e.cellW;
    }
    ++g.d3440;
}

// 1.13c D2Client+0x80E30
void fill(Seed& s, const Env& e) {
    while (g.live < g.t3934) {
        const int32_t before = g.live;
        spawn(s, e);
        if (g.live == before) break;   // pool full: native Ordinal_10064 returns -1 and the count never rises
    }
    const int32_t stepBy = (g.bf78 == 0) + 1;
    if (g.t394c < g.t3918) {
        g.t3918 -= stepBy;
        if (g.t394c > g.t3918) g.t3918 = g.t394c;
    } else if (g.t394c > g.t3918) {
        g.t3918 += stepBy;
        if (g.t3918 > g.t394c) g.t3918 = g.t394c;
    }
    if (--g.t3938 == 0) {
        if (g.bf78 == 0) {
            g.t3938 = int32_t(step(s) % 0x177u) + 0x7d;
            g.t394c = int32_t(step(s) % 0x47u) + 0x5c;
        } else {
            g.t3938 = int32_t(step(s) % 0x36bu) + 0xfa;
            g.t394c = roll(s, 0, 0);
        }
    }
    if (g.bf6c != 0 && --g.t38bc == 0) g.t393c = true;
}

// 1.13c D2Client+0x81750 (the parts that draw or seed the pass)
void setup(Seed& s, const Env& e) {
    levelSetup(s, e);                // 0x814F0
    init80BE0(s);                    // one-time block (DAT_6FBD3444)
    g.tabB[0] = 0x1d4c; g.tabB[1] = 0xfa; g.tabB[2] = 3000; g.tabB[3] = 0x7d;
    if (e.act == 2) g.tabB[2] *= 3;
    if (e.act == 4) g.tabB[2] *= 3;
    g.tabA[0] = 0x1d4c; g.tabA[1] = 0xfa; g.tabA[2] = 3000; g.tabA[3] = 0x32;
    g.f38f4 = 0.0f;
    // Retail's first tick makes 5 further hero-seed draws before 0x811F0 that no recovered caller
    // accounts for (a retail recording trace places the phase-1 duration roll at draw 11, not 6).
    for (int i = 0; i < 5; ++i) step(s);
}

} // namespace

void reset() { g = State{}; }
int32_t liveCount() { return g.live; }
int32_t target() { return g.t3934; }
float intensity() { return g.f38f4; }

void tick(Seed& s, const Env& e) {
    if (!g.setup) { g.setup = true; setup(s, e); }
    if (!e.weatherLevel) { g.f38f4 = 0.0f; return; }
    if (g.live != 0) update(s, e);
    phaseCycle(s, e);
    fill(s, e);
}

} // namespace d2client::weather
