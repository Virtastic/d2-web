// 1.13c D2Client+0x3AB70 -- key command: skill hotkey 9 (needs the skill data).
#include "cl_cmd.h"
extern "C" uint32_t d2c_3ab70(uint32_t, const cl::Env& e) { return clc::hotkey(e, 9, true); }
