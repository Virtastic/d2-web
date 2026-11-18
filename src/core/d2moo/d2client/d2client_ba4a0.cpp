// 1.13c D2Client+0xBA4A0 (ecx = unit): with a room, runs 0xB3D60 twice, 0xB3C40 when the 0xC4D2 value is above 0x3C and a multiple of 0x14, and for value 0x19 two 0xAFF10 calls (0x26A, then 0x269) at the unit's path position + 2 with the 0xC388/0xC3FA coordinates (EDX = room). Always tails to 0xB17C0.
#include "cl_env.h"
extern "C" uint32_t d2c_ba4a0(uint32_t u, const cl::Env& e) {
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    int32_t v = (int32_t)e.H(0xc4d2, u);
    if (room) {
        e.H(0xb3d60, room, 0, 0, 0, 0, u);
        e.H(0xb3d60, room, 0, 0, 0, 0, u);
        if (v > 0x3c && v % 0x14 == 0) e.H(0xb3c40, room, 0, 0, 0, 0, u);
        if (v == 0x19) {
            for (uint32_t t = 0x26a; t >= 0x269; t--) {
                uint32_t y = e.PD(u, 0xc2ec, 0x10), x = e.PD(u, 0xc3be, 0xc);
                uint32_t k = e.H(0xc3fa, u);
                uint32_t j = e.H(0xc388, u);
                e.H(0xaff10, t, x + 2, y + 2, j, k, 0, 0, room);
            }
        }
    }
    return e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
