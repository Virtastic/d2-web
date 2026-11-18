// 1.13c D2Client+0x3AA30 -- key command: skill hotkey 14 (needs the skill data).
#include "cl_cmd.h"
extern "C" uint32_t d2c_3aa30(uint32_t, const cl::Env& e) { return clc::hotkey(e, 0xe, true); }
