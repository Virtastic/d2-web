// 1.13c D2Client+0x3ACF0 -- key command: skill hotkey 3.
#include "cl_cmd.h"
extern "C" uint32_t d2c_3acf0(uint32_t, const cl::Env& e) { return clc::hotkey(e, 3, false); }
