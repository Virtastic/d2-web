// 1.13c D2Client+0x533F0 -- quest effect: raises the effect flag; with the player standing in level 0x6C it spawns
// missile 0x174 at the player and starts the music change (EBX = 0xF3, stack word 1).
#include "cl_pa4.h"
extern "C" uint32_t d2c_533f0(uint32_t eax, const cl::Env& e) {
    using namespace pa4;
    const uint32_t pl = G(e, 0x11bbfc);
    S(e, 0x11c2e4, 1);
    if (!pl) return eax;
    const uint32_t rm = e.H(0xc1f0, pl);
    if (!rm) return 0;
    const uint32_t lv = e.H(0xc1de, rm);
    if (lv != 0x6c) return lv;
    const uint32_t x = ux(e, pl), y = uy(e, pl);
    spawn(e, pl, 0x174, x, y);
    return e.H(0x6a0c0, 1, 0, 0, 0, 0, 0, 0, 0, 0xf3);
}
