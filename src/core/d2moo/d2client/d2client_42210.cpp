// 1.13c D2Client+0x42210 -- key message: steps the byte counter and finishes the message.
#include "cl_pa4.h"
extern "C" uint32_t d2c_42210(uint32_t msg, const cl::Env& e) {
    pa4::S8(e, 0x11c3c8, pa4::G8(e, 0x11c3c8) + 1);
    return pa4::fin(e, msg);
}
