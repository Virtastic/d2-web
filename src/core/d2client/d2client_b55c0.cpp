// 1.13c D2Client+0xb55c0 -- client missile: type-0x21 spawn at the unit's position jittered by +-r on each axis (two LCG draws on the unit seed); EAX = r, ESI = unit, one stack word a (ret 4); returns the spawned child.
#include "cl_env.h"
extern "C" uint32_t d2c_b55c0(uint32_t r, uint32_t u, uint32_t a, const cl::Env& e) {
    auto draw = [&]() -> uint32_t {
        int32_t c = (int32_t)(r + r);
        if (c <= 0) return 0;
        uint32_t lo = e.Lcg(u + 0x20);
        return (c & (c - 1)) == 0 ? (lo & (uint32_t)(c - 1)) : lo % (uint32_t)c;
    };
    uint32_t dx = draw() - r;
    uint32_t dy = draw() - r;
    uint32_t q[23] = {};
    q[0] = 0x21;
    q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    q[5] = e.PD(u, 0xc3be, 0xc);
    q[6] = e.PD(u, 0xc2ec, 0x10);
    q[7] = e.PD(u, 0xc3be, 0xc) + dx;
    q[8] = e.PD(u, 0xc2ec, 0x10) + dy;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[4] = a;
    return e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
}
