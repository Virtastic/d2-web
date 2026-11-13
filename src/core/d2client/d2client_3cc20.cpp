// 1.13c D2Client+0x3CC20 -- activation message: records whether the message's word parameter is nonzero in the
// active flag and leaves the message unhandled. EAX is the message.
#include "cl_pa4.h"
extern "C" uint32_t d2c_3cc20(uint32_t msg, const cl::Env& e) {
    const uint32_t on = e.R(msg + 8) != 0;
    e.W(msg + 0x18, 0);
    pa4::S(e, 0xf4a60, on);
    return msg;
}
