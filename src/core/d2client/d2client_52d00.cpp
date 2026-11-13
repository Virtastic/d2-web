// 1.13c D2Client+0x52D00 -- quest effect: finds the object of class 0x11 in the player's room, spawns missile 0x120
// at its position and starts the screen shake from that missile's table row (EAX = the row's dword at +0x38,
// EBX = its word at +0x96 less twice that dword, stack (4, 1)).
#include "cl_pa4.h"
extern "C" uint32_t d2c_52d00(uint32_t eax, const cl::Env& e) {
    using namespace pa4;
    const uint32_t pl = G(e, 0x11bbfc);
    if (!pl) return eax;
    const uint32_t rm = e.H(0xc1f0, pl);
    if (!rm) return 0;
    const uint32_t u = e.H(0xc334, rm, 2, 0x11);
    if (!u) return 0;
    const uint32_t x = ux(e, u), y = uy(e, u);
    spawn(e, pl, 0x120, x, y);
    const uint32_t row = misRow(e, 0x120);
    const uint32_t d = e.R(row + 0x38);
    return e.H(0x52450, 4, 1, 0, 0, 0, d, 2 * d, 0, (uint32_t)e.R16s(row + 0x96) - 2 * d);
}
