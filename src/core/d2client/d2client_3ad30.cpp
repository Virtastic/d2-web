// 1.13c D2Client+0x3AD30 -- key command: skill hotkey 2.
#include "cl_cmd.h"
extern "C" uint32_t d2c_3ad30(uint32_t, const cl::Env& e) { return clc::hotkey(e, 2, false); }
