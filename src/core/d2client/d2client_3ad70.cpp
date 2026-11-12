// 1.13c D2Client+0x3AD70 -- key command: skill hotkey 1.
#include "cl_cmd.h"
extern "C" uint32_t d2c_3ad70(uint32_t, const cl::Env& e) { return clc::hotkey(e, 1, false); }
