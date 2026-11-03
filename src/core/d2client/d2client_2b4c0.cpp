// 1.13c D2Client+0x2B4C0 -- skill start, the minimal variant: EDI = unit, EDX = skill id, no stack words, plain ret.
// owner = 0xC838(ECX = unit, EDX = skill); flags = 0xC628(owner); bit 0 set: 0xC60A(owner, flags | 2); the result is 0xC30A(unit, 0x36, 0).
#include "cl_mb2.h"
extern "C" uint32_t d2c_2b4c0(uint32_t unit, uint32_t skill, const clm::Env& e) {
    uint32_t owner = e.H(0xc838, 0, unit, skill);
    uint32_t f = e.H(0xc628, 0, 0, 0, 0, 0, owner);
    if (f & 1) e.H(0xc60a, 0, 0, 0, 0, 0, owner, f | 2);
    return e.H(0xc30a, 0, 0, 0, 0, 0, unit, 0x36, 0);
}
