// 1.13c D2Client+0xB1350 (ecx = unit, edx = wanted id): 1 when the unit's 0xA60D0 record is missing or flagged (+0xC4 bit 0x10000), or its (type, state +0x10) pair is
// (0, 0), (0, 0x11), (1, 0), (1, 0xC), or the 0xC1F0 / 0xC208 pair is non-zero. Otherwise by the 0xC322 flag word: bit 0 compares edx with the 0xC2DA answer for the
// unit's +0x2C (0 when they differ and edx is non-zero), bit 2 with edx zero is 1, bit 1 is bit 2 of the word when 0xC406 is positive and 0xB1270(unit) != 0 else.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB1350
extern "C" uint32_t d2c_b1350(uint32_t u, uint32_t edx, const cl::Env& e) {
    const uint32_t path = e.R(u + 0x2c);
    const uint32_t p = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!p || (e.R(p + 0xc4) & 0x10000)) return 1;
    const uint32_t t = e.R(p), st = e.R(p + 0x10);
    if (t == 0 && (st == 0 || st == 0x11)) return 1;
    if (t == 1 && (st == 0 || st == 0xc)) return 1;
    const uint32_t k = e.H(0xc1f0, u);
    if (k && e.H(0xc208, k)) return 1;
    const uint32_t b = e.H(0xc322, u);
    if (!edx && (b & 4)) return 1;
    if (b & 1) {
        if (!edx) return 1;
        return edx == e.H(0xc2da, path) ? 1u : 0u;
    }
    if (b & 2) {
        if ((int32_t)e.H(0xc406, u) > 0) return (b & 0xff) >> 2 & 1;
        return e.H(0xb1270, 0, 0, 0, 0, 0, u) != 0;
    }
    return 1;
}
