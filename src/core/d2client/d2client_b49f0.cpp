// 1.13c D2Client+0xb49f0 -- client missile: reseed, then scatter c322(unit) children (type 0x15d) around the unit; ECX = unit, EDX = skip flag.
#include "cl_env.h"
extern "C" uint32_t d2c_b49f0(uint32_t u, uint32_t skip, const cl::Env& e) {
    if (skip) return 0;
    uint32_t seed = u + 0x20;
    uint32_t a = e.H(0xc4ba, u);
    e.H(0xc3b2, 0, 0, 0, 0, 0, 0, seed, a);
    int32_t n = (int32_t)e.H(0xc322, u);
    uint32_t x = e.PD(u, 0xc3be, 0xc);
    uint32_t y = e.PD(u, 0xc2ec, 0x10);
    uint32_t q[23] = {};
    q[0] = 0x21;
    q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    q[5] = x; q[6] = y; q[4] = 0x15d;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    for (int32_t i = 0; i < n; i++) {
        uint32_t r1 = e.Lcg(seed) % 20;
        uint32_t bx = r1 + x - 10;
        uint32_t r2 = e.Lcg(seed) % 20;
        uint32_t by = y + r2 - 10;
        q[7] = bx; q[8] = by;
        if (bx == x && by == y) { i--; continue; }
        uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        if (m) e.H(0x73b60, 0, 0, 0, 0, 0, 0, m, bx);
    }
    return 1;
}
