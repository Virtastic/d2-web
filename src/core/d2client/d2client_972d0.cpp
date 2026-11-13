// 1.13c D2Client+0x972D0 -- window message handler in screen states 1, 2, 3, 4 and 9 (EAX is the state otherwise): unless the
// quit flag at 0xFAD94 is set (EAX = that flag) it re-reads the cursor item of the player's inventory into 0x11B834, sets
// the cursor mode words, marks the message handled, runs the panel callees and resets the selection, hover and mouse words (EAX = -1).
#include "cl_mouse.h"
extern "C" uint32_t d2c_972d0(uint32_t, uint32_t msg, const cl::Env& e) {
    uint32_t eax = clc::G(e, 0x11BC34);
    if (eax != 1 && eax != 2 && eax != 3 && eax != 4 && eax != 9) return eax;
    eax = clc::G(e, 0xFAD94);
    if (eax) return eax;
    clc::S(e, 0x11BC24, 0);
    eax = e.H(0xC202, e.R(clc::G(e, 0x11BBFC) + 0x60));
    clc::S(e, 0x11B834, eax);
    clc::S(e, 0x11B858, 0);
    clc::S(e, 0x11B854, 5);
    clc::S(e, 0x11B864, eax ? 4u : 1u);
    clm::done(e, msg);
    clc::S(e, 0x11BEEC, 0);
    e.H(0xC2790, 0, 0, 0, 0, 0, 0, 0xC, 1);
    e.H(0x48150);
    clm::pos(e, 0, 0);
    clm::sel(e);
    clm::hov(e);
    return 0xFFFFFFFFu;
}
