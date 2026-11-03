// 1.13c D2Client+0x52570 -- request an overlay at the player's own position: no inputs, EBX = playerSelf ([0x11BBFC]).
// owner = 0xC1F0(self); unit = 0xC334(owner, 1, 0x220); none: return. y then x from the unit's path (0xC2EC, 0xC3BE for
// non-static types), then 0xAFF10(EDX = self; kind 0x271, x, y, 0, 1). The result is ignored.
#include "cl_env.h"
extern "C" void d2c_52570(const cl::Env& e) {
    uint32_t self = e.self;
    uint32_t o = e.H(0xc1f0, self);
    uint32_t u = e.H(0xc334, o, 1, 0x220);
    if (!u) return;
    uint32_t y = e.PD(u, 0xc2ec, 0x10), x = e.PD(u, 0xc3be, 0xc);
    e.H(0xaff10, 0x271, x, y, 0, 1, 0, 0, self);
}
