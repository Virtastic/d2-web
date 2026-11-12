// 1.13c D2Client+0x3ADB0 -- key command: skill hotkey 0.
#include "cl_cmd.h"
extern "C" uint32_t d2c_3adb0(uint32_t, const cl::Env& e) { return clc::hotkey(e, 0, false); }
