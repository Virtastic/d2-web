// 1.13c D2Client+0xb3c40 -- client missile: spawn type 0x11d child at a random offset; EAX = unit, stack (arg), ret 4.
#include "cl_env.h"
extern "C" uint32_t d2c_b3c40(uint32_t u, uint32_t a, const cl::Env& e) {
    uint32_t q[23] = {};
    q[1] = a;
    uint32_t seed = u + 0x20;
    uint32_t r1 = e.Lcg(seed) % 0x28;
    uint32_t dx = 0x14 - r1;
    q[0] = 2; q[2] = u; q[4] = 0x11d;
    uint32_t r2 = e.Lcg(seed) % 0x28;
    uint32_t dy = 0x14 - r2;
    q[7] = dx; q[8] = dy;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    if (dx == 0 && dy == 0) { dx = e.Lcg(seed) % 0x13 + 1; q[7] = dx; }
    uint32_t n = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (!n) return 0;
    e.H(0xc448, n, dx);
    uint32_t r = e.H(0xc394, n, dy);
    uint32_t pa = e.R(n + 0x54);
    if (pa) { uint32_t m = e.R(pa + 0x30); if (m) { e.W(m + 4, 0); e.W(m + 8, 0); e.W(m + 0xc, 0xffffb000u); } }
    return r;
}
