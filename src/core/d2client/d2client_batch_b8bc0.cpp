// src/core/d2client/d2client_batch_b8bc0.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (0xB8BC0..0xB9FF0)
// reimplemented from the cached decompiles. Alpha batch lane: implementation + marker, no spec/vector.
// These are the per-frame "unit anim event" handlers of the missile/object/monster family: each reads a
// unit (+0x04 class, +0x0C id, +0x20/+0x24 seed, +0x2C owner, +0x54/+0x64 sub-objects), consults a
// 0x1A4-byte record in sgptDataTables (+0xB64 base, +0xB6C count), and spawns effect units through
// FUN_6fb630f0 (0x630F0, a 0x17-dword create record). Several decompiles lost register-passed args
// (unaff_*/in_EAX); where the arg is a guess it is marked `guess`.
//
// Env seam: every unrecovered callee goes through D2ClientB8bc0Env. Ordinals are called by number via
// ord(), unrecovered FUN_ by RVA via fn(); both take up to six pointer-width args. Memory is read
// through rd*/wr* so struct layouts stay 32-bit retail layouts.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

struct D2ClientB8bc0Env {
    int64_t (*ord_b8bc0)(uint32_t ordinal, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d);
    int64_t (*fn_b8bc0)(uint32_t rva, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e, uintptr_t f);
    uint32_t (*rd32_b8bc0)(uintptr_t addr);
    int16_t  (*rd16_b8bc0)(uintptr_t addr);
    uint8_t  (*rd8_b8bc0)(uintptr_t addr);
    void     (*wr32_b8bc0)(uintptr_t addr, uint32_t v);
    uintptr_t (*dt_b8bc0)(void);               // *sgptDataTables
    uintptr_t (*img_b8bc0)(uint32_t rva);      // address of a D2Client image global
    uint32_t (*tick_b8bc0)(void);              // GetTickCount
};
static D2ClientB8bc0Env g_e;
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b8bc0_set_env(const D2ClientB8bc0Env* e) { g_e = *e; }

typedef uintptr_t U;
static inline int32_t R(U a) { return (int32_t)g_e.rd32_b8bc0(a); }
static inline void W(U a, uint32_t v) { g_e.wr32_b8bc0(a, v); }
static inline int64_t O(uint32_t n, U a = 0, U b = 0, U c = 0, U d = 0) { return g_e.ord_b8bc0(n, a, b, c, d); }
static inline int64_t F(uint32_t r, U a = 0, U b = 0, U c = 0, U d = 0, U e = 0, U f = 0) { return g_e.fn_b8bc0(r, a, b, c, d, e, f); }
static inline U ctx(U a) { return (U)F(0xA60D0, a); } // FUN_6fb560d0 = RVA 0xA60D0
static inline int32_t ux(U u) { return (int32_t)F(0x1630, u); }   // FUN_6fab1630 unit X (RVA 0x1630)
static inline int32_t uy(U u) { return (int32_t)F(0x1660, u); }
static inline void endFrame(U u) { F(0xB17C0, u); }                      // FUN_6fb617c0
static inline void bail() { F(0xB1720); }                                // FUN_6fb61720
static inline U spawn(uint32_t* rec) { return (U)F(0xB30F0, (U)rec); }   // FUN_6fb630f0
static inline int32_t cls(U u) { return R(u); }
static inline bool isMissileLike(U u) { int32_t t = cls(u); return t == 2 || (t > 3 && t < 6); }
static inline int32_t px(U u) {  // x (path +0x0C) or Ordinal_10867
    if (isMissileLike(u)) return R(R(u + 0x2c) + 0xc);
    return R(u + 0x2c) ? (int32_t)O(10867, R(u + 0x2c)) : 0;
}
static inline int32_t py(U u) {  // y (path +0x10) or Ordinal_10750
    if (isMissileLike(u)) return R(R(u + 0x2c) + 0x10);
    return R(u + 0x2c) ? (int32_t)O(10750, R(u + 0x2c)) : 0;
}
// seed step: seed(u64 at +0x20) = lo*0x6AC690C5 + hi; returns the new low dword
static inline uint32_t nextSeed(U u) {
    uint64_t s = (uint64_t)(uint32_t)R(u + 0x20) * 0x6ac690c5ull + (uint32_t)R(u + 0x24);
    W(u + 0x20, (uint32_t)s); W(u + 0x24, (uint32_t)(s >> 32));
    return (uint32_t)s;
}
static inline U rec(int32_t idx) {  // 0x1A4-byte record or 0
    U dt = g_e.dt_b8bc0();
    if (idx < 0 || idx >= R(dt + 0xb6c)) return 0;
    return (U)R(dt + 0xb64) + (U)idx * 0x1a4;
}
static inline void zero(uint32_t* r) { for (int i = 0; i < 0x17; i++) r[i] = 0; }

// 1.13c D2Client+0xB8BC0: homing-step handler. Pulls the unit toward its target by rec+0x68 per axis,
// then (rec+0x1e, every rec+0x58 frames) spawns a trail effect (type 5).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b8bc0(U unit) {
    U r;
    if (!unit || !(r = rec(R(unit + 4))) || !ctx(R(g_e.dt_b8bc0()))) { bail(); return; }
    int32_t life = (int32_t)O(11018, unit);
    if (life <= -R(r + 0x5c)) { bail(); return; }
    if (life < 1) O(10637, unit, life - 1);
    if (O(10777, R(unit + 0x2c)) != 0) {
        U tgt = (U)O(10392, R(unit + 0x2c));
        if (!tgt) tgt = ctx(0);
        int32_t period = R(r + 0x60); if (period < 1) period = 5;
        const int32_t istack70 = life; // guess: lost local, the life value gates both steps
        if (tgt && O(10737, unit) % period == 0 && istack70 > 0 && O(10522, unit, tgt) < R(r + 100)) {
            int32_t sx = ux(unit), sy = uy(unit), tx = ux(tgt), ty = uy(tgt);
            int32_t st = R(r + 0x68); if (st < 1) st = 1;
            int32_t dx = tx <= sx ? (tx < sx ? -st : 0) : st;
            int32_t dy = ty <= sy ? (ty < sy ? -st : 0) : st;
            O(10286, R(unit + 0x2c), ux(unit) + dx, uy(unit) + dy);
            O(10334, R(unit + 0x2c), unit, 0);
        }
        if (g_e.rd16_b8bc0(r + 0x1e) > 0) {
            int32_t every = R(r + 0x58); if (every < 2) every = 1;
            if (O(10737, unit) % every == 0 && istack70 > 0) {
                uint32_t c[0x17]; zero(c);
                c[0] = 5; c[3] = (uint32_t)ux(unit); c[4] = (uint32_t)uy(unit);
                c[5] = (uint32_t)g_e.rd16_b8bc0(r + 0x1e);
                c[6 + 4] = (uint32_t)O(10240, unit); c[6 + 5] = (uint32_t)O(11029, unit);
                U e = spawn(c);
                if (e) O(10637, e, 0);
            }
        }
    }
    endFrame(unit);
}

// 1.13c D2Client+0xB8DF0: spawn a scattered ring effect (type 4). Register args lost; `period` in EAX,
// `spread` on the stack, unit in EBX (guess: all three read from the caller frame).
extern "C" EMSCRIPTEN_KEEPALIVE U d2_client_b8df0(int32_t period, int32_t spread, U unit) {
    if (period <= 0 || O(10985, unit) % period != 0) return 0;
    if (!ctx(0) || !O(10331, unit)) return 0;
    uint32_t c[0x17]; zero(c);
    c[0] = 0; // guess: Ordinal_10614 result lands in the record head, see decompile
    c[0x0a] = (uint32_t)O(10240, unit); c[0x0b] = (uint32_t)O(11029, unit);
    uint32_t rx = (uint32_t)F(0x1380 /*FUN_6fab1380*/, unit + 0x20), ry = (uint32_t)F(0x1380, unit + 0x20);
    int32_t X = ux(unit) + ((spread - 1) - (int32_t)rx), Y = uy(unit) + ((spread - 1) - (int32_t)ry);
    if ((int16_t)O(10482, 0, (U)X, (U)Y, 4) == 0) return spawn(c);
    return 0;
}

// 1.13c D2Client+0xB8F00: timed spawn (type 1) at the unit with rec+0x1e (and +0x20) effect ids, plus
// frame-count-gated side effects at the final frame.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b8f00(U unit) {
    U r = unit ? rec(R(unit + 4)) : 0;
    if (!r || g_e.rd16_b8bc0(r + 0x1e) < 0) { bail(); return; }
    int32_t d = (int32_t)O(10240, unit);
    U dt = g_e.dt_b8bc0();
    if (d < 0 || d >= R(dt + 0xba0) || !(R(dt + 0xb98) + (U)d * 0x23c)) { bail(); return; }
    U row = (U)R(dt + 0xb98) + (U)d * 0x23c;
    int32_t a = R(r + 0x58); if (a < 1) a = 1;
    int32_t b = R(r + 0x5c) > 0 ? R(r + 0x5c) : 1; b *= a;
    int32_t c2 = R(r + 0x60) > 0 ? R(r + 0x60) : 1;
    int32_t rowh = c2 * a;
    int32_t z = (int32_t)O(11029);
    (void)b;
    int32_t n = (int32_t)O(10786, ctx(0), R(row + 0x138), d);
    if (n < 2) n = 1; else if (n > 0x3b) n = 0x3c;
    U sub = (U)R(unit + 0x64);
    U h = sub ? (U)F(0xA8710) : 0;
    int32_t fr = (int32_t)O(10985, unit);
    if ((fr + 1) % c2 == 0 && n > 0 /* guess: unaff_ESI */ && sub) F(0xA8B60);
    if (fr == 0) {
        uint32_t cr[0x17]; zero(cr);
        cr[0] = 1; cr[1] = (uint32_t)ctx(0); cr[3] = (uint32_t)ux(unit); cr[4] = (uint32_t)uy(unit);
        cr[5] = (uint32_t)g_e.rd16_b8bc0(r + 0x1e);
        cr[10] = (uint32_t)O(10240, unit); cr[11] = (uint32_t)O(11029, unit);
        for (int pass = 0; pass < 2; pass++) {
            if (pass) { if (g_e.rd16_b8bc0(r + 0x20) <= 0) break; cr[5] = (uint32_t)g_e.rd16_b8bc0(r + 0x20); }
            U e = spawn(cr);
            if (e) {
                F(0xC7E30, h, 0, (U)(-rowh));
                U o = (U)R(e + 0x54);
                if (o && (o = (U)R(o + 0x30))) { W(o + 0x10, (uint32_t)unit << 11); W(o + 0x14, 0); W(o + 0x18, (uint32_t)(z * -0x800)); }
            }
        }
    }
    if (fr == (int32_t)unit - 2) {   // guess: decompile compares with param_1 - 2
        if (F(0x22320)) F(0x88C20, unit, 1);
        if (g_e.rd16_b8bc0(r + 0x34) > 0) F(0x88A70, unit, 0, 0, 0);
    }
    endFrame(unit);
}

// 1.13c D2Client+0xB91C0: once the unit reaches frame rec+0x135, spawn a type-0 record.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b91c0(U unit) {
    U r = unit ? rec(R(unit + 4)) : 0;
    if (!r || g_e.rd16_b8bc0(r + 0x1e) < 0) { bail(); return; }
    if ((int32_t)g_e.rd8_b8bc0(r + 0x135) <= (int32_t)O(10985, unit) && O(10203, R(unit + 0x2c))) {
        uint32_t c[0x17]; zero(c);
        c[1] = (uint32_t)ctx(0); c[3] = (uint32_t)ux(unit); c[4] = (uint32_t)uy(unit);
        c[10] = (uint32_t)O(10240, unit); c[11] = (uint32_t)O(11029, unit);
        spawn(c);
    }
    endFrame(unit);
}

// 1.13c D2Client+0xB92B0: spawn the rec+0x1e effect with seed-jittered timing, then copy two unit
// scalars (Ordinals 11080/11168) onto the new unit's sub-object.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b92b0(U unit) {
    ctx(unit);
    if (!O(10203, R(unit + 0x2c))) { endFrame(unit); return; }
    U r = rec(R(unit + 4));
    if (!r || g_e.rd16_b8bc0(r + 0x1e) < 0) { bail(); return; }
    uint32_t c[0x17]; zero(c);
    c[3] = (uint32_t)ux(unit); c[4] = (uint32_t)uy(unit);
    c[10] = (uint32_t)O(10240, unit); c[11] = (uint32_t)O(11029, unit);
    int32_t id = g_e.rd16_b8bc0(r + 0x1e);
    if (!rec(id)) { bail(); return; }
    if (g_e.rd16_b8bc0(r + 0x20) >= 0) F(0x1330, unit + 0x20, 0); // guess: rolls consumed, results lost
    if (R(r + 0x58)) F(0x1330, unit + 0x20, 0);
    U e = spawn(c);
    if (e) {
        U s = (U)R(e + 0x2c);
        O(10236, s, (U)O(11080, unit));
        O(10575, s, (U)O(11168, unit));
    }
    endFrame(unit);
}

// 1.13c D2Client+0xB9470: burst spawn (type 0), `count` records at seed-jittered offsets. Args lost:
// unit in EBX, idx = 3rd stack arg, count = [esp+4]; guess order (unit, idx, scale, count).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b9470(U unit, int32_t idx, int32_t scale, int32_t count) {
    U dt = g_e.dt_b8bc0();
    if (idx < 0 || idx >= R(dt + 0xb6c)) return;
    if (O(10737, unit) != 0 && F(0x1380, unit + 0x20) != 0) return;
    uint32_t c[0x17]; zero(c);
    c[1] = (uint32_t)ctx(0); c[10] = (uint32_t)O(10240, unit); c[11] = (uint32_t)O(11029, unit);
    c[3] = (uint32_t)px(unit);
    if (!isMissileLike(unit) && R(unit + 0x2c)) O(10750, R(unit + 0x2c));
    for (int32_t i = 0; i < count; i++) {
        uint32_t m = (uint32_t)scale * 2;
        for (int k = 0; k < 2; k++) if ((int32_t)m > 0) nextSeed(unit);
        spawn(c);
    }
}

// 1.13c D2Client+0xB9650: spawn a type-5 record (flag 0x8000 if a>0, |8 if b>0). Args in EBX/stack/ESI.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b9650(U unit, int32_t flagHi, int32_t flagLo) {
    if (!O(10203, R(unit + 0x2c))) return;
    uint32_t c[0x17]; zero(c);
    c[0] = 5; if (flagHi > 0) c[0] = 0x8005; if (flagLo > 0) c[0] |= 8;
    c[1] = (uint32_t)ctx(0);
    c[3] = (uint32_t)px(unit); c[4] = (uint32_t)py(unit); // guess: filled by the lost-arg stores
    c[10] = (uint32_t)O(10240, unit); c[11] = (uint32_t)O(11029, unit);
    spawn(c);
}

// 1.13c D2Client+0xB9740: scatter pass over a (2r+1)^2 disc of tiles around the unit; each walkable
// tile (Ordinal_11076 clear, Ordinal_10057 zero) may spawn a type-1 record with probability param_3 %.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b9740(U unit, int32_t r, int32_t zOff, int32_t pct, int32_t heavy) {
    if (!O(10331, unit)) return;
    uint32_t c[0x17]; zero(c);
    c[0] = heavy ? 0x8001 : 1;
    c[1] = (uint32_t)ctx(0);
    c[10] = (uint32_t)O(10240, unit); c[11] = (uint32_t)O(11029, unit);
    int32_t cx = px(unit), cy = py(unit);
    for (int32_t dy = -r; dy <= r; dy++)
        for (int32_t dx = -r; dx <= r; dx++) {
            U room = (U)O(10331, unit);
            if (!room || dx * dx + dy * dy > r * r) continue;
            int32_t box[4] = { cx + dy, cy + dx, cx + dy + dy, cy + dx + dx }; // guess: (x0,y0,x1,y1)
            if (O(11076, room, (U)&box[0], (U)&box[2], 4) != 0) continue;
            U cell = (U)F(0x1400, room);
            if (!cell || O(10057, cell) != 0) continue;
            if (zOff) c[9] = (uint32_t)((nextSeed(unit) % 25) - 12 + zOff);
            c[6] = (uint32_t)(cx + dy); c[7] = (uint32_t)(cy + dx);
            uint32_t k = (uint32_t)F(0x1330, unit + 0x20);
            c[2] = R(cell + k * 4);
            if ((int32_t)(nextSeed(unit) % 100) < pct) spawn(c);
        }
}

// 1.13c D2Client+0xB99D0: create a type-0x420 record at the (game or unit) context; returns the unit.
extern "C" EMSCRIPTEN_KEEPALIVE U d2_client_b99d0(U ctxIn) {
    if (!ctxIn) return 0;
    if (R(ctxIn) != 0 && R(ctxIn) != 1) { ctxIn = ctx(ctxIn); if (!ctxIn) return 0; }
    int32_t out[0x18]; // local_60: filled by FUN_6fb51340
    if (!F(0xA1340, (U)out)) return 0;
    uint32_t c[0x17]; zero(c);
    c[0] = 0x420; c[2] = (uint32_t)ctxIn;
    return spawn(c);
}

// 1.13c D2Client+0xB9A80: spawn a type-0x521 record at unit + (dx,dy). thiscall: ECX=this, EAX=unit.
extern "C" EMSCRIPTEN_KEEPALIVE U d2_client_b9a80(U unit, U self, uint32_t p1, int32_t dx, int32_t dy,
                                                  int32_t p4, int32_t p5, uint32_t p6, uint32_t p7, int32_t p8) {
    if (!unit) return 0;
    U cx = unit;
    if (R(unit) != 0 && R(unit) != 1) { cx = ctx(self); if (!cx) return 0; }
    if (!(p4 && p5)) { int32_t out = 0; if (!F(0xA1340, (U)&out)) return 0; p5 = out; }
    if (!(p4 && p5)) return 0;
    if (!p8) p8 = 0x1e;
    uint32_t c[0x17]; zero(c);
    c[0] = 0x521; c[1] = (uint32_t)cx; c[4] = p1;
    c[5] = (uint32_t)(ux(unit) + dx); c[6] = (uint32_t)(uy(unit) + dy);
    c[7] = (uint32_t)p4; c[8] = (uint32_t)p5; c[9] = (uint32_t)p8; c[10] = p6; c[11] = p7;
    return spawn(c);
}

// 1.13c D2Client+0xB9C10: death/impact burst keyed on the unit's frame count `f`. f==0 -> big burst;
// f<0x32 -> 8 puffs, <0x50 -> 5, else 1 (all with the rec 0x255 effect); f==5 arms the screen-shake
// globals; every 7th frame (<200) calls FUN_6fb639e0.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b9c10(U unit) {
    U g = ctx(unit);
    int32_t f = (int32_t)O(10985, unit);
    int32_t X = px(unit), Y = py(unit);
    if (f < 0x6e) {
        int32_t n;
        if (f == 0) {
            F(0xAFF10, (U)Y, g, 0x214, (U)X, (U)Y, (U)O(10240, unit, (U)O(11029)));
            n = 8;
        } else if (f < 0x32) n = 8; else n = (f < 0x50) * 4 + 1;
        for (; n; n--) {
            uint32_t d = (uint32_t)O(11029, unit), t = (uint32_t)O(10240, unit);
            if (!g) continue;
            uint32_t c[0x17]; zero(c);
            c[0] = 1; c[1] = (uint32_t)g; c[4] = 0x255; c[5] = (uint32_t)(X + 2); c[6] = (uint32_t)(Y + 2);
            c[10] = t; c[11] = d;
            U e = spawn(c);
            if (e) {
                uint32_t s1 = nextSeed(unit), s2 = nextSeed(unit), s3 = nextSeed(unit);
                U o = (U)R(e + 0x54);
                if (o && (o = (U)R(o + 0x30))) {
                    W(o + 4, (uint32_t)((0x32 - (int32_t)(s1 % 100)) * 0x800));
                    W(o + 8, (uint32_t)((0x32 - (int32_t)(s2 % 100)) * 0x800));
                    W(o + 0xc, 0);
                    W(o + 0x10, 0); W(o + 0x14, 0);
                    W(o + 0x18, (uint32_t)(((s3 & 7) + 2) * 0x800));
                }
            }
        }
    }
    if (f == 5) {
        W(g_e.img_b8bc0(0x10B9CC), g_e.tick_b8bc0());
        W(g_e.img_b8bc0(0x10B9D0), 600);
        W(g_e.img_b8bc0(0x10B9D4), 4000);
        W(g_e.img_b8bc0(0x10B9D8), 3000);
        W(g_e.img_b8bc0(0x11BEFC), 3);
    } else if (f > 199) { endFrame(unit); return; }
    if (f % 7 == 0) F(0xB39E0, g, (U)X);
    endFrame(unit);
}

// 1.13c D2Client+0xB9FF0: per-frame effect for the 0x145/0x177/0x1a9/0x1c2 frame milestones. A 4-row
// {maxFrame, period, enabled} table at +0xD35D4 picks the smoke-puff cadence; the milestone frames
// spawn 0x27a/0x279/0x278/0x277 via FUN_6fb5ff10; a flagged unit also pings FUN_6fb63b40.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b9ff0(U unit) {
    U g = ctx(unit);
    if (!g) { endFrame(unit); return; }
    int32_t f = (int32_t)O(10737, unit);
    U tab = g_e.img_b8bc0(0xD35D4);
    int32_t row = 0;
    for (row = 0; row < 4 && R(tab + row * 12) < f; row++) {}
    // retail loop: stop at the first row whose limit >= f (or run off the end at row 4)
    int32_t period = row < 4 ? R(tab + row * 12 + 4) : 1, on = row < 4 ? R(tab + row * 12 + 8) : 0;
    if (period && f % period == 0 && on) {
        nextSeed(unit);
        uint32_t c[0x17]; zero(c);
        c[0] = 2; c[4] = 0x272;
        c[9] = (nextSeed(unit) % 0x28) - 0x14; c[10] = (nextSeed(unit) % 0x28) - 0x14;
        c[2] = (uint32_t)unit; c[12] = (uint32_t)O(10240, unit); c[14] = (uint32_t)O(11029, unit);
        U e = spawn(c);
        if (e) O(10637, e, unit ? R(unit + 0xc) : -1);
    }
    O(10637, unit, f < 0xd2 ? 2 : 1);
    uint32_t id = f == 0x145 ? 0x27a : f == 0x177 ? 0x279 : f == 0x1a9 ? 0x278 : f == 0x1c2 ? 0x277 : 0;
    if (id) {
        int32_t y = py(unit), x = px(unit);
        if (f == 0x1c2) { y++; x++; }
        F(0xAFF10, g, 0, id, (U)x, (U)y, (U)O(10240, unit, (U)O(11029)));
    }
    if (O(10818, unit)) {
        U ru = (U)O(10754, O(10331, unit, 1, 0x209));
        if (ru) {
            uint32_t s = nextSeed(unit);
            F(0xB3B40, (U)(px(ru) - 5 + (int32_t)(s % 10)));
        }
    }
    endFrame(unit);
}
