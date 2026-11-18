// 1.13c D2Client+0x39CA0 -- key command: send the three-byte message {0x3F, 0x001F}.
#include "cl_cmd.h"
extern "C" uint32_t d2c_39ca0(uint32_t, const cl::Env& e) { return clc::send3f(e, 0x1f); }
