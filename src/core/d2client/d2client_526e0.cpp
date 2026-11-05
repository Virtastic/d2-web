// 1.13c D2Client+0x526E0 -- unit event request 0x161 (EAX = unit): y then x from the unit's path (static types 2, 4, 5 read
// path+0x10 / +0xC, the rest call 0xC2EC / 0xC3BE; no path: 0); no playerSelf ([0x11BBFC]): nothing more. Otherwise 23
// zeroed words, [0] = 1, [1] = playerSelf, [4] = 0x161, [5] = x, [6] = y, [12] = 1, handed to 0xB30F0.
#include "cl_env.h"
extern "C" void d2c_526e0(uint32_t u, const cl::Env& e) {
    uint32_t y = e.PD(u, 0xc2ec, 0x10), x = e.PD(u, 0xc3be, 0xc);
    if (!e.self) return;
    uint32_t q[23] = {};
    q[0] = 1; q[1] = e.self; q[4] = 0x161; q[5] = x; q[6] = y; q[12] = 1;
    e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
}
