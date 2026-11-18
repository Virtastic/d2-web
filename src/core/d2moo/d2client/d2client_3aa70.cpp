// 1.13c D2Client+0x3AA70 -- key command: skill hotkey 13 (needs the skill data).
#include "cl_cmd.h"
extern "C" uint32_t d2c_3aa70(uint32_t, const cl::Env& e) { return clc::hotkey(e, 0xd, true); }
