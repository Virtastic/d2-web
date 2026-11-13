// 1.13c D2Client+0x9FC50 -- stdcall(msg): Storm-ordinal 10085(-1, 0, 0) with EDX = -1, ECX = 0; then FUN 0x26320 (EAX = msg);
// when the UI mode [0x10B214] is 1, FUN 0x94FA0 (EDX = msg, EAX = the 0x26320 result). Returns the last callee's EAX.
#include "cl_cmd.h"
extern "C" uint32_t d2c_9fc50(uint32_t msg, const cl::Env& e) {
    e.H(0xd384, 0xffffffffu, 0, 0, 0, 0, 0, 0, 0xffffffffu);
    uint32_t a = e.H(0x26320, 0, 0, 0, 0, 0, msg);
    if (clc::G(e, 0x10B214) == 1) a = e.H(0x94fa0, 0, 0, 0, 0, 0, a, 0, msg);
    return a;
}
