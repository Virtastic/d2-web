// 1.13c D2Client+0xb5540 -- client missile: spawn a type-0 (0x54) child at the unit, stamp frame word and path flag; EAX = a, ECX = unit; returns the child or 0.
#include "cl_env.h"
extern "C" uint32_t d2c_b5540(uint32_t u, uint32_t a, const cl::Env& e) {
    uint32_t p = e.R(u + 0x2c);
    if (!p) return 0;
    uint32_t b = e.H(0xc2d4, p) & 0xff;
    uint32_t q[23] = {};
    q[1] = u; q[2] = u; q[4] = 0x54; q[12] = 1;
    uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (!m) return 0;
    e.W(m + 0x44, a << 8);
    uint32_t mp = e.R(m + 0x2c);
    if (mp) e.H(0xc2b6, mp, b);
    e.W(m + 0xc4, e.R(m + 0xc4) | 0x10000);
    return m;
}
