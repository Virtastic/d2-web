// 1.13c D2Client+0x7E910 -- a message whose dword 8 is 0x73 is left alone (result = the message); any other goes to the
// table check at 0x7E8A0 (tail call). Stack: message (ret 4).
#include "cl_env.h"
extern "C" uint32_t d2c_7e910(uint32_t msg, const cl::Env& e) {
    if (e.R(msg + 8) == 0x73) return msg;
    return e.H(0x7e8a0, msg);
}
