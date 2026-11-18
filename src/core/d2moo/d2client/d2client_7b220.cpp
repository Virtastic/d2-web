// 1.13c D2Client+0x7B220 -- message handler: marks the message handled (+0x18 = 1, +0x1C = 0) and tail-jumps to the shared message tail.
#include "cl_pa4.h"
extern "C" uint32_t d2c_7b220(uint32_t msg, const cl::Env& e) { return pa4::fin(e, msg); }
