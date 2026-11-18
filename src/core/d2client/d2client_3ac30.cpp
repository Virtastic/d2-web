// 1.13c D2Client+0x3AC30 -- key command: skill hotkey 6.
#include "cl_cmd.h"
extern "C" uint32_t d2c_3ac30(uint32_t, const cl::Env& e) { return clc::hotkey(e, 6, false); }
