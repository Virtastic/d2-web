// 1.13c D2Client+0x3ABF0 -- key command: skill hotkey 7.
#include "cl_cmd.h"
extern "C" uint32_t d2c_3abf0(uint32_t, const cl::Env& e) { return clc::hotkey(e, 7, false); }
