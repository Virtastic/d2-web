// 1.13c D2Client+0x6F040 -- message handler that marks the message handled with result 0. EAX is the message.
#include "cl_pa4.h"
extern "C" uint32_t d2c_6f040(uint32_t msg, const cl::Env& e) {
    e.W(msg + 0x18, 1);
    e.W(msg + 0x1c, 0);
    return msg;
}
