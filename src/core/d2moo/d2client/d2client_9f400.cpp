// 1.13c D2Client+0x9F400 -- stdcall(msg): marks the message handled (msg+0x18 = 1, msg+0x1C = 0) and tail-jumps to Storm 511
// (thunk 0xBE84) with it; the thunk's return value is the function's.
#include "cl_env.h"
extern "C" uint32_t d2c_9f400(uint32_t msg, const cl::Env& e) {
    e.W(msg + 0x18, 1); e.W(msg + 0x1c, 0);
    return e.H(0xbe84, msg);
}
