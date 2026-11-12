// 1.13c D2Client+0x3ACB0 -- key command: skill hotkey 4.
#include "cl_cmd.h"
extern "C" uint32_t d2c_3acb0(uint32_t, const cl::Env& e) { return clc::hotkey(e, 4, false); }
