// 1.13c D2Client+0x90FD0 -- window message handler: with the panel flag at 0x119854 set, a message without the 0x40000000
// bit (and no quit flag at 0xFAD94) runs the panel routine, then the message is marked handled. Without the flag EAX is the flag.
#include "cl_mouse.h"
extern "C" uint32_t d2c_90fd0(uint32_t, uint32_t msg, const cl::Env& e) {
    const uint32_t f = clc::G(e, 0x119854);
    if (!f) return f;
    if (!(e.R(msg + 0xC) & 0x40000000u) && !clc::G(e, 0xFAD94)) e.H(0x90F60);
    return clm::done(e, msg);
}
