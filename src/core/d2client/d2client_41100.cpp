// 1.13c D2Client+0x41100 -- client missile disc scatter: EAX = unit, EDX = seed init value (hidden registers), stack a0..a5 (ret 0x18),
// no return value.
// A local LCG seed is set up by #C3B2(ECX = &seed, EDX). Request block (23 dwords, zeroed): [0] = 1, [1] = unit, [4] = a0, [11] = a4, [12] = a5.
// room = #C1F0(unit). With r = a3 and n = r*r (32-bit), n times (nothing when n <= 0): tx = r - roll, ty = r - roll where roll draws one LCG step
// on the local seed and takes (lo & (2r-1)) for a power-of-two 2r or lo % 2r, and is 0 (no step) when 2r <= 0; a point with tx*tx + ty*ty > n is
// dropped (signed compare). Else [5] = tx + a1, [6] = ty + a2 (EBX = x, EDI = y) and when #1400(EAX = room) is non-zero, #B30F0(ECX = block) runs.
#include "cl_env.h"
extern "C" void d2c_41100(uint32_t unit, uint32_t edx, uint32_t a0, uint32_t a1, uint32_t a2, int32_t r, uint32_t a4, uint32_t a5,
                          const cl::Env& e) {
    uint32_t sd[2] = {};
    uint32_t sp = (uint32_t)(uintptr_t)sd;
    e.H(0xc3b2, 0, 0, 0, 0, 0, 0, sp, edx);
    int32_t n = (int32_t)((uint32_t)r * (uint32_t)r);
    uint32_t q[23] = {};
    q[0] = 1; q[1] = unit; q[4] = a0; q[11] = a4; q[12] = a5;
    uint32_t room = e.H(0xc1f0, unit);
    if (n <= 0) return;
    uint32_t d = (uint32_t)r * 2;
    auto roll = [&]() -> uint32_t {
        if ((int32_t)d < 1) return 0;
        uint32_t lo = e.Lcg(sp);
        return (d & (d - 1)) == 0 ? ((d - 1) & lo) : lo % d;
    };
    for (int32_t c = n; c != 0; c--) {
        uint32_t tx = (uint32_t)r - roll();
        uint32_t ty = (uint32_t)r - roll();
        if ((int32_t)(ty * ty + tx * tx) > n) continue;
        uint32_t x = tx + a1, y = ty + a2;
        q[5] = x; q[6] = y;
        if (e.H(0x1400, 0, 0, 0, 0, 0, room, 0, 0, x)) e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
}
