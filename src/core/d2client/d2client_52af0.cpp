// 1.13c D2Client+0x52AF0 -- quest effect: finds the monster of class 0x20E in the player's room and spawns the two
// effect missiles (0x25E, 0x25B) at its position, then marks the unit (bit 0x40000 of +0xC8).
#include "cl_pa4.h"
extern "C" uint32_t d2c_52af0(uint32_t eax, const cl::Env& e) {
    using namespace pa4;
    const uint32_t pl = G(e, 0x11bbfc);
    if (!pl) return eax;
    const uint32_t rm = e.H(0xc1f0, pl);
    if (!rm) return 0;
    const uint32_t u = e.H(0xc334, rm, 1, 0x20e);
    if (!u) return 0;
    const uint32_t x = ux(e, u), y = uy(e, u);
    spawn(e, pl, 0x25e, x, y);
    spawn(e, pl, 0x25b, x, y);
    const uint32_t v = e.R(u + 0xc8) | 0x40000u;
    e.W(u + 0xc8, v);
    return v;
}
