// 1.13c D2Client+0xb3b40 -- client missile: spawn type 0x2001 child; EAX = missile unit, EBX = y, stack (x), ret 4. Returns the last call's EAX.
#include "cl_env.h"
extern "C" uint32_t d2c_b3b40(uint32_t u, uint32_t y, uint32_t x, const cl::Env& e) {
    uint32_t g = e.H(0xc1f0, u);
    uint32_t r = e.H(0xc34c, g, x, y, 3, 1);
    if (r & 0xffff) return r;
    uint32_t q[23] = {};
    q[0] = 0x2001;
    q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    q[4] = 0x27b + e.Lcg(u + 0x20) % 3;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[5] = x; q[6] = y;
    uint32_t n = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (!n) return 0;
    uint32_t pa = e.R(n + 0x54);
    if (pa) { uint32_t m = e.R(pa + 0x30); if (m) { e.W(m + 4, 0); e.W(m + 8, 0); e.W(m + 0xc, 0x7d000); } }
    pa = e.R(n + 0x54);
    if (pa) { uint32_t m = e.R(pa + 0x30); if (m) { e.W(m + 0x10, 0); e.W(m + 0x14, 0); e.W(m + 0x18, 0xffffb000u); } }
    e.H(0xc4a2, n, 0x19);
    e.H(0xc2c8, n, 0x19);
    return e.H(0xc382, n, 0x80);
}
