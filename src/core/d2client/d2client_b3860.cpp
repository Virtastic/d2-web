// 1.13c D2Client+0xb3860 -- client missile: every third tick scatter a child; ECX = missile unit; returns the B17C0 result.
#include "cl_env.h"
extern "C" uint32_t d2c_b3860(uint32_t u, const cl::Env& e) {
    int32_t t = (int32_t)e.H(0xc4d2, u);
    if (t % 3 == 0) {
        uint32_t seed = u + 0x20;
        uint32_t r1 = e.Lcg(seed) % 20;
        uint32_t r2 = e.Lcg(seed) % 20;
        auto stat = [&]() { int32_t ty = (int32_t)e.R(u); return ty == 2 || (ty > 3 && ty <= 5); };
        auto dim = [&](uint32_t rva, uint32_t off) {
            uint32_t p = e.R(u + 0x2c);
            if (stat()) return e.R(p + off);
            return p ? e.H(rva, p) : 0u;
        };
        uint32_t d1 = dim(0xc3be, 0xc);
        uint32_t bx = (e.Lcg(seed) & 1) + d1 - 1;
        uint32_t d2 = dim(0xc2ec, 0x10);
        uint32_t by = (e.Lcg(seed) & 1) + d2 - 1;
        uint32_t q[23] = {};
        q[0] = 0x21;
        uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
        q[1] = room;
        q[5] = bx; q[6] = by; q[7] = r1 + bx - 10; q[8] = by + r2 - 10;
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        q[4] = 0x21e;
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
    return e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
