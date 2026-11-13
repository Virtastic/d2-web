// 1.13c D2Client+0x42490 -- message handler that only finishes the message (handled, result 0, shared tail).
#include "cl_pa4.h"
extern "C" uint32_t d2c_42490(uint32_t msg, const cl::Env& e) { return pa4::fin(e, msg); }
