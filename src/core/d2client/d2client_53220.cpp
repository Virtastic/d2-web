// 1.13c D2Client+0x53220 -- quest effect: finds the object of class 0x178 in the player's room, posts the timed
// message, plays sound 0x23 on the object and spawns missile 0x175 at its position, with the object's id as the
// missile's range word and a cleared target.
#include "cl_pa4.h"
extern "C" uint32_t d2c_53220(uint32_t eax, const cl::Env& e) {
    using namespace pa4;
    const uint32_t pl = G(e, 0x11bbfc);
    if (!pl) return eax;
    const uint32_t rm = e.H(0xc1f0, pl);
    if (!rm) return 0;
    const uint32_t u = e.H(0xc334, rm, 2, 0x178);
    if (!u) return 0;
    e.H(0x45570, 0x1251, 0, 0x15e, 0, 0);
    e.H(0x88a70, u, 0, 0, 0, 0, 0, 0, 0, 0x23);
    const uint32_t x = ux(e, u), y = uy(e, u);
    const uint32_t r = spawn(e, pl, 0x175, x, y);
    if (!r) return 0;
    e.H(0xc448, r, e.R(u + 0xc));
    return e.H(0xc394, r, 0);
}
