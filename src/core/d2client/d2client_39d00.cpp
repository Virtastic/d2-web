// 1.13c D2Client+0x39D00 -- key command: send the three-byte message {0x3F, 0x001C}.
#include "cl_cmd.h"
extern "C" uint32_t d2c_39d00(uint32_t, const cl::Env& e) { return clc::send3f(e, 0x1c); }
