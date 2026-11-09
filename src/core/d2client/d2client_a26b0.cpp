// 1.13c D2Client+0xA26B0: EAX = unit, ESI = ctx0, stack p1 p2 p3 dx dy ctx1 (ret 0x18)..
// Zeroes a 23-word block; w0 = 0x21, w1 = unit, w4 = p1, w11 = p2, w12 = p3, w5/w6 = unit x/y + dx/dy (path+0xC/+0x10 for unit types 2, 4, 5;
// else 0 without a path, else the ordinals 10867 / 10750 = rva 0xC3BE / 0xC2EC), (w7, w8) = (ESI, ctx1) or what 0xA1340 fills.
#include "cl_env.h"
extern "C" uint32_t d2cl_a26b0(uint32_t unit, uint32_t esi, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t dx, uint32_t dy,
                               uint32_t ctx1, const cl::Env& e) {
    uint32_t m[23] = {0};
    m[0] = 0x21;
    m[5] = e.PD(unit, 0xc3be, 0xc) + dx;
    m[6] = e.PD(unit, 0xc2ec, 0x10) + dy;
    m[1] = unit; m[4] = p1; m[11] = p2; m[12] = p3;
    uint32_t pair[2] = {0, 0};   // block words 7, 8: zero until 0xA1340 fills them
    if (esi == 0 || ctx1 == 0) {
        e.Ed((uint32_t)(uintptr_t)&pair[0]);
        if (!e.H(0xa1340, (uint32_t)(uintptr_t)&pair[1], 0, 0, 0, 0, 0, 0, 0, unit)) return 0;
    } else { pair[0] = esi; pair[1] = ctx1; }
    m[7] = pair[0]; m[8] = pair[1];
    if (pair[0] != 0 && pair[1] != 0) return e.HB(0xb30f0, (uint32_t)(uintptr_t)m);
    return 0;
}
