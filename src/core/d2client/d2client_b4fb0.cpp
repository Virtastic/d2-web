// 1.13c D2Client+0xb4fb0 -- client missile: ring of 14 type-0x8102 children (class 0x176 row speed scaled by an octagonal distance); EBX = unit, stack (a), ret 4. Result not defined by the code (EAX is scratch at exit).
#include "cl_env.h"
extern "C" uint32_t d2c_b4fb0(uint32_t u, uint32_t a, const cl::Env& e) {
    static const int32_t X[14] = {-4, 2, 4, -3, -2, 3, 5, -3, -5, 5, -6, -1, 0, 6};
    static const int32_t Y[14] = {6, 4, -4, -2, 1, 7, 0, 3, -4, -3, 4, -5, 5, -3};
    uint32_t row = e.Row(0x176);
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!room) return 0;
    uint32_t q[23] = {};
    q[0] = 0x8102; q[4] = 0x176; q[1] = room; q[2] = u;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    for (int k = 0; k < 14; k++) {
        int32_t x = X[k], y = Y[k];
        q[7] = (uint32_t)x; q[8] = (uint32_t)y;
        int32_t ax = x < 0 ? -x : x, ay = y < 0 ? -y : y;
        int32_t d = (x < y ? ay + 2 * ax : ax + 2 * ay) >> 1;
        int32_t sp = e.R16s(row + 0x96);
        q[19] = (uint32_t)((int32_t)((uint32_t)sp * (uint32_t)d) / 6);
        uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        if (m) {
            e.H(0xc448, m, a);
            uint32_t r = e.Lcg(u + 0x20) % 0x23 + 10;
            e.H(0xc394, m, r);
        }
    }
    return 0;
}
