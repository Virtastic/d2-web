// 1.13c D2Client+0x52C50 -- quest effect: finds the object of class 0x95 in the player's room, spawns missile 0x15E
// at its position and aims the missile at the object's id.
#include "cl_pa4.h"
extern "C" uint32_t d2c_52c50(uint32_t eax, const cl::Env& e) {
    using namespace pa4;
    const uint32_t pl = G(e, 0x11bbfc);
    if (!pl) return eax;
    const uint32_t rm = e.H(0xc1f0, pl);
    const uint32_t u = e.H(0xc334, rm, 2, 0x95);
    if (!u) return 0;
    const uint32_t y = e.PD(u, 0xc2ec, 0x10), x = e.PD(u, 0xc3be, 0xc);
    const uint32_t r = spawn(e, pl, 0x15e, x, y);
    if (!r) return 0;
    return e.H(0xc394, r, e.R(u + 0xc));
}
