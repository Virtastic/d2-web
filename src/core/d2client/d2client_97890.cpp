// 1.13c D2Client+0x97890 -- window message handler in screen state 0xE: marks the message handled; unless the flag at
// 0xFAD84 is set resets the selection and hover words (EAX = -1); unless the quit flag at 0xFAD94 is set calls one routine.
#include "cl_mouse.h"
extern "C" uint32_t d2c_97890(uint32_t eax, uint32_t msg, const cl::Env& e) {
    if (clc::G(e, 0x11BC34) != 0xE) return eax;
    eax = clm::done(e, msg);
    if (!clc::G(e, 0xFAD84)) {
        eax = 0xFFFFFFFFu;
        clm::sel(e);
        clm::hov(e);
    }
    if (clc::G(e, 0xFAD94)) return eax;
    return e.H(0x97770);
}
