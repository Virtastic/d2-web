// src/core/d2client/sound_tick_88dd0.cpp -- 1.13c D2Client sound-entry per-frame update (list at DAT_6fbcbf30).
// Env-seam style of d2client_batch_87de0.cpp: entry fields at retail byte offsets (packed, unaligned),
// list node = {entry*, next*}, record table stride 0x8E. x87 float10 sums are done in float (ceiling:
// last-bit differences in the averaged distance).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

struct D2cSnd88dd0 {
    uint32_t enable;        // +0x10BF34
    uint32_t pending;       // +0x10BF40 (-> sub_88D90)
    void**   list;          // +0x10BF30
    uint8_t* recs;          // +0x10C24C
    int32_t  count;         // +0x10C250
    int32_t  lo, hi;        // +0x10C25C / +0x10C260
    uint32_t g3a0;          // +0x10C3A0
    uint32_t g394;          // +0x10C394
    uint32_t a_da4, a_dac;  // +0x0AADA4 / +0x0AADAC
    int32_t  g268;          // +0x10C268
    uint32_t lastTick;      // +0x10BF3C
    int32_t  vol374;        // +0x0F1374 (DAT_6fb91374)
    int32_t  vol378;        // +0x0F1378 (DAT_6fb91378)
    float    step;          // _DAT_6fb875ac
    float    zero;          // _DAT_6fb87578
    float    range[5];      // 6fb87554 (case 0), 6fb87544 (default), 6fb87550 (2), 6fb8754c (3), 6fb87548 (4)
};
struct D2cSnd88dd0Env {
    void     (*fn88d90)(void);
    void     (*fn880e0)(uint8_t* e);
    int32_t  (*ord10001)(void);
    int32_t  (*ord10011)(void);
    void     (*ord10034)(void);
    void     (*ord10062)(void);
    void     (*ord10018)(void);
    void     (*ord10063)(void);
    void     (*fn87f30)(void* item);
    float    (*fn88440)(void* item);
    void     (*fn88350)(float* acc, uint8_t* e);
    uint8_t* (*fn87c30)(int32_t grp, void* item, uint8_t* e);
    uint8_t* (*fn87bd0)(uint8_t* e);
    uint32_t*(*fn0d7a0)(uint8_t* e);
    void     (*fn0d1d0)(void);
    void     (*fatal)(uint32_t code); // Ordinal_10265 + Ordinal_10024 + _exit(-1)
};

template <class T> static T rd(const uint8_t* p, size_t o) { T v; memcpy(&v, p + o, sizeof v); return v; }
template <class T> static void wr(uint8_t* p, size_t o, T v) { memcpy(p + o, &v, sizeof v); }
static uint8_t* rec88(const D2cSnd88dd0* s, int32_t i) {
    static uint8_t zero[0x100];  // retail dereferences address 0 here; model as zeros
    return (!s->recs || i < 0 || i >= s->count) ? zero : s->recs + (size_t)i * 0x8E;
}
static uint8_t* ent88(void** n) { return (uint8_t*)n[0]; }

// 1.13c D2Client+0x88DD0 (now = param_1)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_88dd0(D2cSnd88dd0* s, const D2cSnd88dd0Env* v, uint32_t now) {
    if (!s->enable) return;
    if (s->pending) v->fn88d90();
    // bubble sort of the entry pointers
    for (bool done = false; !done;) {
        done = true;
        if (!s->list) break;
        for (void** n = s->list; n; n = (void**)n[1]) {
            void** m = (void**)n[1];
            if (!m) continue;
            uint8_t* a = ent88(n); uint8_t* b = ent88(m);
            uint8_t ca = rd<uint8_t>(a, 0x3c), cb = rd<uint8_t>(b, 0x3c);
            float fa = rd<float>(a, 0x18), fb = rd<float>(b, 0x18);
            uint32_t ta = rd<uint32_t>(a, 0x34), tb = rd<uint32_t>(b, 0x34);
            if (ca < cb || (ca <= cb && (fb < fa || (fb <= fa && (ta < tb || (ta <= tb && a < b)))))) {
                n[0] = b; m[0] = a; done = false;
            }
        }
    }
    for (void** n = s->list; n; n = (void**)n[1]) {
        uint8_t* e = ent88(n);
        if (rd<int32_t>(e, 0x3d) != 0 && (rd<int32_t>(e, 0x45) == 0 || rd<int32_t>(e, 0x4d) != 0)) {
            if (rd<int32_t>(e, 0x30) == 0) wr<int32_t>(e, 0x30, 2);
            if (rd<int32_t>(e, 0x30) == 1) v->fn880e0(e);
        }
    }
    bool b2 = v->ord10001() > 0;
    for (void** n = s->list; n; n = (void**)n[1]) {
        uint8_t* e = ent88(n);
        int32_t idx = rd<int32_t>(e, 4);
        uint8_t* r = rec88(s, idx);
        bool b5 = b2;
        if (s->lo <= idx && idx <= s->hi && v->ord10011() == 0) b5 = false;
        if (rd<int32_t>(e, 0x30) == 2) {
            if (rd<uintptr_t>(e, 0x2c)) { v->fatal(0x349); return; }
            if (rd<uint8_t>(r, 0x3e) == 0 || rd<uint16_t>(r, 0x43) != 0 || rd<int32_t>(e, 0x3d) != 0) {
                v->ord10034();
            } else {
                if (rd<int32_t>(e, 0x45)) { wr<uint32_t>(e, 0x1c, rd<uint32_t>(e, 0x4d)); wr<int32_t>(e, 0x45, 0); }
                wr<int32_t>(e, 0x30, 0);
            }
            continue;
        }
        if (s->g3a0 && rd<uint8_t>(r, 0x51) && rd<uintptr_t>(e, 0x28)) {
            float acc[3] = {0, 0, 0}; void* accItem = nullptr; float accF = -1.0f;
            float sum = 0; int32_t cnt = 0;
            for (void** it = (void**)rd<uintptr_t>(e, 0x28); it; it = (void**)it[1]) {
                void* item = it[0];
                if (!item) { v->fatal(0x36c); return; }
                v->fn87f30(item);
                if (!accItem || accF > 0.0f) { acc[0] = acc[1] = acc[2] = 0; accItem = item; accF = 0.0f; }
                sum += v->fn88440(item);
                ++cnt;
            }
            v->fn88350(acc, e);
            float avg = sum / (float)cnt, cur = rd<float>(e, 0x20), nv;
            if (avg <= cur) { nv = cur - s->step; if (nv <= avg) nv = avg; }
            else { nv = cur + s->step; if (avg <= nv) nv = avg; }
            wr<float>(e, 0x20, nv);
        }
        if (rd<int32_t>(e, 0x45)) {
            if (rd<uint32_t>(e, 0x55) < now) { wr<uint32_t>(e, 0x1c, rd<uint32_t>(e, 0x4d)); wr<int32_t>(e, 0x45, 0); }
            else {
                uint32_t t0 = rd<uint32_t>(e, 0x51); int32_t val;
                if (now < t0) val = rd<int32_t>(e, 0x49);
                else val = (int32_t)((uint32_t)(rd<int32_t>(e, 0x4d) - rd<int32_t>(e, 0x49)) * (now - t0))
                           / (int32_t)(rd<uint32_t>(e, 0x55) - t0) + rd<int32_t>(e, 0x49);
                wr<int32_t>(e, 0x1c, val);
            }
        }
        r = rec88(s, rd<int32_t>(e, 4));
        float rg;
        switch (rd<int32_t>(r, 0x47)) {
            case 0: rg = s->range[0]; break;
            case 2: rg = s->range[2]; break;
            case 3: rg = s->range[3]; break;
            case 4: rg = s->range[4]; break;
            default: rg = s->range[1]; break;
        }
        rg *= rg;
        if (rd<int32_t>(e, 0x30) == 0 && rd<uint32_t>(e, 0x34) <= now && rd<int32_t>(e, 0x1c) != 0) {
            if (b5 && rd<float>(e, 0x18) < rg) {
                if (rd<uintptr_t>(e, 0x2c)) { v->fatal(0x3a1); return; }
                void** head = (void**)rd<uintptr_t>(e, 0x28);
                void* single = (head && !head[1]) ? head[0] : nullptr;
                bool b4 = !(rd<float>(e, 0xc) == s->zero && rd<float>(e, 0x10) == s->zero && rd<float>(e, 0x14) == s->zero);
                uint8_t* o = nullptr;
                if (rd<uint8_t>(r, 0x41) || rd<uint8_t>(r, 0x42)) o = v->fn87c30(rd<int32_t>(r, 0x60), single, e);
                int32_t id = rd<int32_t>(e, 4);
                if (single && 0xb75 < id && id < 0x1231) o = v->fn87bd0(e);
                if ((rd<uint8_t>(r, 0x41) == 0 || rd<uintptr_t>(e, 0x28) != 0 || !b4) && o) {
                    if (rd<uint8_t>(r, 0x42) == 0) { wr<int32_t>(e, 0x3d, 1); v->ord10034(); continue; }
                    wr<int32_t>(o, 0x3d, 1); v->ord10034();
                }
                int32_t q = rd<int32_t>(e, 0x41);
                if (q) { wr<int32_t>(e, 0x41, 0); wr<int32_t>(e, 0x24, q); }
                uint32_t keep = 0, dur = 0; bool go = false;
                if (q == 0) {
                    if (rd<uint8_t>(r, 0x3f) && rd<int32_t>(e, 0x45) == 0 && (rd<uint8_t>(e, 0x38) & 2) == 0) { dur = rd<uint8_t>(r, 0x3f); go = true; }
                } else { dur = 3; go = true; }
                if (go) { keep = rd<uint32_t>(e, 0x1c); wr<uint32_t>(e, 0x1c, 0); }
                uint32_t* h = v->fn0d7a0(e);
                wr<uintptr_t>(e, 0x2c, (uintptr_t)h);
                if (h) {
                    wr<int32_t>(e, 0x30, 1);
                    if (dur) {
                        wr<uint32_t>(e, 0x49, rd<uint32_t>(e, 0x1c)); wr<uint32_t>(e, 0x4d, keep);
                        wr<uint32_t>(e, 0x51, now); wr<uint32_t>(e, 0x55, dur + now); wr<int32_t>(e, 0x45, 1);
                    }
                }
                r = rec88(s, rd<int32_t>(e, 4));
            }
            if (rd<uintptr_t>(e, 0x2c) == 0 && rd<uint8_t>(r, 0x3e) == 0 &&
                (rd<uint8_t>(r, 0x4d) == 0 || rd<int32_t>(r, 0x86) != 1))
                v->ord10034();
        }
        if (rd<int32_t>(e, 0x30) == 1) {
            if (rd<uintptr_t>(e, 0x2c) == 0) { v->fatal(0x3f3); return; }
            uint16_t w = rd<uint16_t>(r, 0x43);
            if ((w != 0 && w < now - rd<uint32_t>(e, 0x34)) || !b5 || rg < rd<float>(e, 0x18)) {
                v->fn880e0(e);
            } else {
                v->fn0d1d0();
                if (rd<int32_t>(r, 0x86) == 2) {
                    wr<int32_t>(rec88(s, rd<int32_t>(e, 4)), 0x7c, s->g268);
                    v->ord10062(); v->ord10018();
                }
            }
        }
    }
    v->ord10063();
    bool b2b = (s->g394 == 0 || s->g394 == 1) && (s->a_da4 != 0 || s->a_dac != 0);
    bool b5 = false;
    for (void** n = s->list; n; n = (void**)n[1]) {
        uint8_t* e = ent88(n);
        if (rd<uint8_t>(rec88(s, rd<int32_t>(e, 4)), 0x52) && (rd<int32_t>(e, 0x45) == 0 || rd<int32_t>(e, 0x4d) != 0)) { b5 = true; break; }
    }
    if (s->lastTick < now) {
        if (b2b) s->vol374 = s->vol374 - 5 < 0 ? 0 : s->vol374 - 5;
        else { int32_t u = s->vol374 + 5; s->vol374 = u < 0x65 ? u : 100; }
        if (b5) { s->vol378 -= 2; if (s->vol378 < 0x46) s->vol378 = 0x46; }
        else { s->vol378 += 2; if (100 < s->vol378) s->vol378 = 100; }
    }
    s->lastTick = now;
}
