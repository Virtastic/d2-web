// 1.13c D2Client+0x57F60 -- skill do: the row's missile created count times at the unit's position toward the
// resolved target point (request kind 0x21, the iteration in the last word).
#include "cl_skdo.h"
extern "C" uint32_t d2c_57f60(uint32_t u, int32_t sk, uint32_t lvl, const cl::Env& e) {
    const uint32_t r = clsk::row(e, sk);
    if (!r) return 0;
    const int32_t m = e.R16s(r + 0xea);
    if (!clsk::mis(e, m)) return 0;
    clsk::acted(e, u);
    const int32_t n = (int32_t)e.H(0xc316, u, e.R(r + 0x138), (uint32_t)sk, lvl);
    uint32_t d[23] = {0};
    d[0] = 0x21; d[1] = u;
    d[5] = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
    d[6] = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
    d[12] = lvl; d[4] = (uint32_t)m; d[21] = 0xB2000000u | 0x56D00;
    e.Ed((uint32_t)(uintptr_t)&d[7]);
    if (!e.H(0xa1340, (uint32_t)(uintptr_t)&d[8], 0, 0, 0, 0, 0, 0, 0, u)) return 0;
    for (int32_t i = 0; i < n; ++i) {
        d[22] = (uint32_t)i;
        e.HB(0xb30f0, (uint32_t)(uintptr_t)d);
    }
    return 1;
}
