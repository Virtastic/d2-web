// 1.13c D2Client+0x39D60 -- key command: send the three-byte message {0x3F, 0x0019}.
#include "cl_cmd.h"
extern "C" uint32_t d2c_39d60(uint32_t, const cl::Env& e) { return clc::send3f(e, 0x19); }
