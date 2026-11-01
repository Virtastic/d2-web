// 1.13c D2Client+0xb5b70 -- client missile: n jittered type-0x524 children around the unit plus a fixed 18-point ring of type-1 children; EAX = n, ECX = unit, stack A B C (ret 0xc).
#include "cl_env.h"
extern "C" uint32_t d2c_b5b70(uint32_t n, uint32_t u, uint32_t A, int32_t B, int32_t C, const cl::Env& e) {
    static const int32_t tx[18] = {2, -2, 0, 0, -3, 0, 3, -1, 1, -1, 2, -4, -3, -1, 0, 1, 3, 4};      // 1.13c D2Client+0xd3770
    static const int32_t ty[18] = {-2, -2, 2, 5, 3, 3, 3, 2, 1, -1, -1, -2, -2, -3, -4, -3, -3, -2};   // 1.13c D2Client+0xd3728
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!room) return 0;
    uint32_t c388 = e.H(0xc388, u), c3fa = e.H(0xc3fa, u);
    uint32_t x = e.PD(u, 0xc3be, 0xc), y = e.PD(u, 0xc2ec, 0x10);
    uint32_t r = 0;
    if (B >= 0 && (int32_t)n > 0) {
        uint32_t q[23] = {};
        q[0] = 0x524; q[1] = room; q[2] = u; q[4] = 0x1c8; q[10] = A; q[11] = c388; q[12] = c3fa;
        uint32_t w = n + n;
        int32_t md = (int32_t)(w + w + 1);
        auto draw = [&]() -> uint32_t {
            if (md <= 0) return 0;
            uint32_t lo = e.Lcg(u + 0x20);
            return (md & (md - 1)) == 0 ? (lo & (uint32_t)(md - 1)) : lo % (uint32_t)md;
        };
        for (uint32_t i = n; i; --i) {
            uint32_t X = draw() - w + x;
            uint32_t Y = draw() - w + y;
            uint32_t k = e.H(0xc4d8, u, X, Y);
            if ((int32_t)k > 3) { q[7] = X; q[8] = Y; r = e.HB(0xb30f0, (uint32_t)(uintptr_t)q); }
        }
    }
    if (C < 0) return r;
    for (int k = 0; k < 18; ++k) {
        uint32_t q[23] = {};
        q[0] = 1; q[1] = room; q[4] = (uint32_t)C;
        q[5] = (uint32_t)tx[k] + x; q[6] = (uint32_t)ty[k] + y;
        q[11] = c388; q[12] = c3fa;
        r = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
    return r;
}
