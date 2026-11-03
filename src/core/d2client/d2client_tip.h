// Seam for the item-tooltip range builders D2Client 0x2E410 / 0x2E900: table reads and the C316 / 2D220 calls go through cl::Env,
// the C244 thunk (six stack words, the last an out-pointer) and the three module globals ride here.
#pragma once
#include "cl_env.h"
struct D2cTipEnv {
    const cl::Env* e;
    uint32_t cls, gmode, g2;   // D2Client+0x11C390 (byte), +0x11C394, +0x119858
    void* ctx;
    void (*c244)(void* ctx, uint32_t idx, uint32_t g, uint32_t cls, uint32_t word, uint32_t mode, int32_t* out);
};
// v*s/100 as retail picks it by magnitude: 32-bit product, 64-bit alldiv, or the divide-first form.
inline int32_t d2c_tip_pct(int32_t v, int32_t s) {
    auto m32 = [](int32_t a, int32_t b) { return (int32_t)((uint32_t)a * (uint32_t)b); };
    if (v > 0x100000) {
        if ((v & ~0xf) < 0x640) return (int32_t)((int64_t)s * v / 100);
        return m32(v / 100, s);
    }
    if (s > 0x10000) {
        if ((s & ~0xf) < 0x640) return (int32_t)((int64_t)s * v / 100);
        return m32(s / 100, v);
    }
    return m32(s, v) / 100;
}
// shared body: kind6f selects 0x2E410 (stat 0x6F entries add to both bounds)
inline uint32_t d2c_tip_body(const D2cTipEnv& t, uint32_t a1, uint32_t dst, uint32_t a3, uint32_t item, uint32_t a5, bool kind6f) {
    const cl::Env& e = *t.e;
    const int32_t idx = e.R16s(item + 0xbc);
    if (idx < 0 || idx >= (int32_t)e.R(e.dt + 0xa80)) return 0;
    const uint32_t rec = (uint32_t)idx * 0x1a8 + e.R(e.dt + 0xa78);
    if (!rec) return 0;
    int32_t loc[14] = {};
    const uint32_t g = (t.gmode < 2 || t.gmode > 3) ? 1u : t.g2;
    t.c244(t.ctx, (uint32_t)idx, g, t.cls & 0xff, (uint32_t)e.R16s(rec + (t.cls & 0xff) * 2 + 0xaa), 8, loc);
    int32_t A = loc[5], B = loc[6], sum = 0;
    auto scan = [&](uint32_t kinds, uint32_t vals, int n) {
        for (int i = 0; i < n; ++i) {
            const uint32_t k = e.R(kinds + i * 2) & 0xffff;
            if (k == 0x19) sum = (int32_t)((uint32_t)sum + e.H(0xc316, a1, e.R(vals + i * 4), a3, a5));
            if (kind6f && k == 0x6f) { const int32_t v = (int32_t)e.H(0xc316, a1, e.R(vals + i * 4), a3, a5); A = (int32_t)((uint32_t)A + (uint32_t)v); B = (int32_t)((uint32_t)B + (uint32_t)v); }
        }
    };
    scan(item + 0x54, item + 0x68, 6);
    scan(item + 0x98, item + 0xa4, 5);
    if (sum != 0) {
        if (sum <= -90) sum = -90;
        A = (int32_t)((uint32_t)A + (uint32_t)d2c_tip_pct(A, sum));
        B = (int32_t)((uint32_t)B + (uint32_t)d2c_tip_pct(B, sum));
    }
    e.H(0x2d220, dst, (uint32_t)A, (uint32_t)B, 0, 0, 0x10a0);
    return 1;
}
