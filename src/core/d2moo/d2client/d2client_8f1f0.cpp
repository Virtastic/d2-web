// 1.13c D2Client+0x8F1F0 -- window message handler: in screen state 0xC, marks the message handled, and unless the
// quit flag at 0xFAD94 is set resets the hover and selection words. EAX is -1 after the reset, else the handled call's answer.
#include "cl_mouse.h"
extern "C" uint32_t d2c_8f1f0(uint32_t eax, uint32_t msg, const cl::Env& e) {
    if (clc::G(e, 0x11BC34) != 0xC) return eax;
    eax = clm::done(e, msg);
    if (clc::G(e, 0xFAD94)) return eax;
    e.H(0x8CCB0);
    clm::sel(e);
    clm::hov(e);
    return 0xFFFFFFFFu;
}
