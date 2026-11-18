// 1.13c D2Client+0x3ABB0 -- key command: skill hotkey 8.
#include "cl_cmd.h"
extern "C" uint32_t d2c_3abb0(uint32_t, const cl::Env& e) { return clc::hotkey(e, 8, false); }
