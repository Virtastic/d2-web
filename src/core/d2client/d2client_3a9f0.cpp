// 1.13c D2Client+0x3A9F0 -- key command: skill hotkey 15 (needs the skill data).
#include "cl_cmd.h"
extern "C" uint32_t d2c_3a9f0(uint32_t, const cl::Env& e) { return clc::hotkey(e, 0xf, true); }
