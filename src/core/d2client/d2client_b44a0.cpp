// 1.13c D2Client+0xb44a0 -- client missile: every 4th tick spawn a 0x2521 child near the unit; ECX = unit; returns the B17C0 result.
#include "cl_env.h"
extern "C" uint32_t d2c_b44a0(uint32_t u, const cl::Env& e) {
    int32_t t = (int32_t)e.H(0xc406, u);
    if (t % 4 == 0) {
        uint32_t q[23] = {};
        q[0] = 0x2521;
        q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
        q[4] = 0x162;
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        uint32_t x = e.PD(u, 0xc3be, 0xc);
        uint32_t y = e.PD(u, 0xc2ec, 0x10);
        q[5] = x; q[6] = y;
        uint32_t r1 = e.Lcg(u + 0x20) % 12;
        q[7] = x - r1;
        uint32_t r2 = e.Lcg(u + 0x20) % 13;
        q[8] = r2 + y - 6;
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
    return e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
