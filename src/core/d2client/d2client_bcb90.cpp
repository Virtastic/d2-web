// 1.13c D2Client+0xBCB90 -- vendor sell chance percent for a unit. Stack arg 0 = the unit (ret 4); null: 0.
// gl block: +0 the player 0x11BBFC, +4 the byte 0x11C390 (difficulty index), +8 0x11C394, +0xC the vendor class id 0xF2188,
// +0x10 0x119854, +0x14 0x119858.
// Index >= 3: Fog #10265(0x18E), #10024(msg, it), exit(-1). Row = the 0x1A8-stride table at [dt+0xA78] (count [dt+0xA80]) by the
// class id; out of range or null: Fog #10265(0x193), #10024, exit(-1). One #11089(id, mode, idx, s16 row+0xAA+2*idx, 2, &out)
// (0xC244; the 14-dword out block zeroed first; mode = [0x119858] when 2 <= [0x11C394] <= 3, else 1); v = out[2]. Row byte
// +0x4C != 1, [0x119854] == 0 and the index byte non-zero: v = v*10/12. c = BC720(unit in EAX, player in ECX) (a + record word);
// v < 0 moves -v onto c (v = 0), then c < 0 moves -c onto v (c = 0); pct = 100 when both are 0, else c*100/(v+c).
// Result k*pct*2 / (cap + k), k = #10973(pl, 0xC, 0) (0xC1C6), clamped to [5, 0x5F]; 32-bit wrap, signed division.
#include <cstdint>
#include "cl_env.h"
extern "C" int32_t d2c_bcb90(const cl::Env& e, uint32_t unit) {
    if (!unit) return 0;
    const uint32_t g = e.gl;
    uint32_t pl = e.R(g);
    uint32_t idx = e.R8(g + 4);
    if (idx >= 3) {
        uint32_t m = e.H(0xbf92, 0x18e);
        e.H(0xbf80, 0xB2000000u | 0xd359fu, m);
        e.H(0x3664, 0xffffffffu);
        return 0;   // exit(-1) does not return
    }
    int32_t id = (int32_t)e.R(g + 0xc);
    uint32_t row = 0;
    if (id >= 0 && id < (int32_t)e.R(e.dt + 0xa80)) row = e.R(e.dt + 0xa78) + (uint32_t)id * 0x1a8;
    if (!row) {
        uint32_t m = e.H(0xbf92, 0x193);
        e.H(0xbf80, 0xB2000000u | 0xd359fu, m);
        e.H(0x3664, 0xffffffffu);
        return 0;
    }
    int32_t cap = e.R16s(row + 0xaa + idx * 2);
    int32_t ks = (int32_t)e.R(g + 8);
    uint32_t mode = (ks >= 2 && ks <= 3) ? e.R(g + 0x14) : 1;
    uint32_t out[14] = {};
    uint32_t w[6] = { (uint32_t)id, mode, idx, (uint32_t)cap, 2, (uint32_t)(uintptr_t)out };
    e.Xs(w[5]);
    e.H(0xc244, w[0], w[1], w[2], w[3], w[4]);
    if (e.blk) e.blk(e.ctx, 0xc244, (uint32_t)(uintptr_t)w, 6);
    int32_t a = (int32_t)out[2];
    if (e.R8(row + 0x4c) != 1 && e.R(g + 0x10) == 0 && e.R8(g + 4) != 0) a = (int32_t)((uint32_t)a * 10u) / 12;
    int32_t c = (int32_t)e.H(0xbc720, 0, 0, 0, 0, 0, unit, pl);
    if (a < 0) { c = (int32_t)((uint32_t)c - (uint32_t)a); a = 0; }
    int32_t pct = 100;
    if (c < 0) { a = (int32_t)((uint32_t)a - (uint32_t)c); c = 0; if (a != 0) pct = (int32_t)((uint32_t)c * 100u) / (int32_t)((uint32_t)a + (uint32_t)c); }
    else if (c == 0) { if (a != 0) pct = 0; }
    else pct = (int32_t)((uint32_t)c * 100u) / (int32_t)((uint32_t)a + (uint32_t)c);
    int32_t k = (int32_t)e.H(0xc1c6, pl, 0xc, 0);
    int32_t den = (int32_t)((uint32_t)cap + (uint32_t)k);
    int32_t o = den ? (int32_t)((uint32_t)k * (uint32_t)pct * 2u) / den : 0;
    if (o < 5) return 5;
    return o > 0x5f ? 0x5f : o;
}
