// 1.13c D2Client+0x581F0 -- skill do: a ring of the row's missile. Eight requests (kind 0x1F) take their offsets from
// the two module tables, then, with the missile row's second range, every count-th entry of the shifted tables.
#include "cl_skdo.h"
extern "C" uint32_t d2c_581f0(uint32_t u, int32_t sk, uint32_t lvl, const cl::Env& e) {
    const uint32_t r = clsk::row(e, sk);
    if (!r) return 0;
    const int32_t m = e.R16s(r + 0xea);
    if (!clsk::mis(e, m)) return 0;
    const uint32_t mrow = e.R(e.dt + 0xb64) + (uint32_t)m * 0x1a4;
    if (!mrow) return 0;
    clsk::acted(e, u);
    uint32_t d[23] = {0};
    d[0] = 0x1f; d[1] = u; d[2] = u; d[4] = (uint32_t)m;
    d[5] = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
    d[6] = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
    d[12] = lvl;
    d[13] = e.H(0xc316, u, e.R(r + 0x138), (uint32_t)sk, lvl);
    d[10] = e.R(mrow + 0x38) << 6;
    int32_t n = (int32_t)e.H(0xc316, u, e.R(r + 0x13c), (uint32_t)sk, lvl);
    if (n < 1) n = 1;
    for (uint32_t off = 0; off < 0x40; off += 8) {
        d[7] = clc::G(e, 0xD5C38 + off);
        d[8] = clc::G(e, 0xD5BF8 + off);
        e.HB(0xb30f0, (uint32_t)(uintptr_t)d);
    }
    d[10] = e.R(mrow + 0x3c) << 6;
    if (n > 1) {
        const int32_t step = (int32_t)((uint32_t)n << 2);
        for (int32_t off = 0; off < 0x3c; off += step) {
            d[8] = clc::G(e, 0xD5BFC + (uint32_t)off);
            d[7] = clc::G(e, 0xD5C3C + (uint32_t)off);
            e.HB(0xb30f0, (uint32_t)(uintptr_t)d);
        }
    }
    return 1;
}
