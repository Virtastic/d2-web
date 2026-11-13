// 1.13c D2Client+0x52B70 -- quest effect: finds the object of class 0x1CC in the player's room, spawns missile
// 0x210 at its position and sets the missile's range to 0x19.
#include "cl_pa4.h"
extern "C" uint32_t d2c_52b70(uint32_t eax, const cl::Env& e) {
    using namespace pa4;
    const uint32_t pl = G(e, 0x11bbfc);
    if (!pl) return eax;
    const uint32_t rm = e.H(0xc1f0, pl);
    if (!rm) return 0;
    const uint32_t u = e.H(0xc334, rm, 2, 0x1cc);
    if (!u) return 0;
    const uint32_t y = uy(e, u), x = ux(e, u);
    const uint32_t r = spawn(e, pl, 0x210, x, y);
    if (!r) return 0;
    return e.H(0xc448, r, 0x19);
}
