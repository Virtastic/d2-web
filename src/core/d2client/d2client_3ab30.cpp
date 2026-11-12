// 1.13c D2Client+0x3AB30 -- key command: skill hotkey 10 (needs the skill data).
#include "cl_cmd.h"
extern "C" uint32_t d2c_3ab30(uint32_t, const cl::Env& e) { return clc::hotkey(e, 0xa, true); }
