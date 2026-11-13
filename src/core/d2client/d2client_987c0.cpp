// 1.13c D2Client+0x987C0 -- window message handler: a message without the 0x40000000 bit runs one routine first, then
// the message is marked handled. EAX is the handled call's answer.
#include "cl_mouse.h"
extern "C" uint32_t d2c_987c0(uint32_t, uint32_t msg, const cl::Env& e) {
    if (!(e.R(msg + 0xC) & 0x40000000u)) e.H(0x3B300);
    return clm::done(e, msg);
}
