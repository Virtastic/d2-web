// 1.13c D2Client+0xbc520 -- vendor/gamble chance base for the player. No args, plain ret, returns a signed int.
// gl block: +0 the player 0x11BBFC, +4 the byte 0x11C390 (difficulty-style index), +8 0x11C394, +0xC the row id 0xF218C,
// +0x10 0x119854, +0x14 0x119858.
// tot = D2Common #10672(pl) (thunk 0xC2A4) + #10973(pl, 0x21, 0) (0xC1C6). Index >= 3: Fog #10265(0x1D6), #10024(msg, it),
// exit(-1) (0x3664; the process ends). Row = the 0x1A8-stride table at [dt+0xA78] (count [dt+0xA80]) by 0xF218C; none: 0.
// Up to three #11089(id, mode, idx, s16 row+0xAA+2*idx, flag, &out) (0xC244; flags 8, 0x10, 0x20; the 14-dword out block zeroed before
// each; mode = [0x119858] when 2 <= [0x11C394] <= 3, else 1) until out+0xC is non-zero; tot2 = out+0xC.
// Row byte +0x4C != 1, [0x119854] == 0 and the index byte non-zero: tot2 = tot2*10/15. c = tot, a = tot2; c < 0 moves
// -c onto a (c = 0), then a < 0 moves -a onto c (a = 0); pct = 100 when both are 0, else a*100/(c+a).
// Result (v*pct*2) / (#10973(pl, 0xC, 0) + v), v the row word; 32-bit wrap, signed division throughout.
#include <cstdint>
#include "cl_env.h"
extern "C" int32_t d2c_bc520(const cl::Env& e) {
    const uint32_t g = e.gl;
    uint32_t pl = e.R(g);
    int32_t tot = (int32_t)(e.H(0xc2a4, pl) + e.H(0xc1c6, pl, 0x21, 0));
    uint32_t idx = e.R8(g + 4);
    if (idx >= 3) {
        uint32_t m = e.H(0xbf92, 0x1d6);
        e.H(0xbf80, 0xB2000000u | 0xd359fu, m);
        e.H(0x3664, 0xffffffffu);
        return 0;   // exit(-1) does not return
    }
    int32_t id = (int32_t)e.R(g + 0xc);
    if (id < 0 || id >= (int32_t)e.R(e.dt + 0xa80)) return 0;
    uint32_t row = e.R(e.dt + 0xa78) + (uint32_t)id * 0x1a8;
    if (!row) return 0;
    int32_t v = e.R16s(row + 0xaa + idx * 2);
    int32_t t2 = 0;
    static const uint32_t fl[3] = { 8, 0x10, 0x20 };
    for (int k = 0; k < 3; ++k) {
        uint32_t out[14] = {};
        int32_t ks = (int32_t)e.R(g + 8);
        uint32_t mode = (ks >= 2 && ks <= 3) ? e.R(g + 0x14) : 1;
        uint32_t w[6] = { (uint32_t)id, mode, idx, (uint32_t)v, fl[k], (uint32_t)(uintptr_t)out };
        e.Xs(w[5]);
        e.H(0xc244, w[0], w[1], w[2], w[3], w[4]);
        if (e.blk) e.blk(e.ctx, 0xc244, (uint32_t)(uintptr_t)w, 6);
        t2 = (int32_t)out[3];
        if (t2) break;
    }
    if (e.R8(row + 0x4c) != 1 && e.R(g + 0x10) == 0 && e.R8(g + 4) != 0) t2 = (int32_t)((uint32_t)t2 * 10u) / 15;
    int32_t c = tot, a = t2, pct;
    if (c < 0) { a = (int32_t)((uint32_t)a - (uint32_t)c); c = 0; }
    if (a < 0) { c = (int32_t)((uint32_t)c - (uint32_t)a); a = 0; }
    if (a == 0 && c == 0) pct = 100;
    else pct = (int32_t)((uint32_t)a * 100u) / (int32_t)((uint32_t)c + (uint32_t)a);
    int32_t w2 = (int32_t)e.H(0xc1c6, pl, 0xc, 0);
    return (int32_t)((uint32_t)v * (uint32_t)pct * 2u) / (int32_t)(uint32_t)((uint32_t)w2 + (uint32_t)v);
}
