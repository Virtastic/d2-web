// 1.13c D2Client+0x3AC70 -- key command: skill hotkey 5.
#include "cl_cmd.h"
extern "C" uint32_t d2c_3ac70(uint32_t, const cl::Env& e) { return clc::hotkey(e, 5, false); }
