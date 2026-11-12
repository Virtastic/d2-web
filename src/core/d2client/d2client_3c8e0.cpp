// 1.13c D2Client+0x3C8E0 -- key command: belt slot 3 (flag byte 0x11C487).
#include "cl_cmd.h"
extern "C" uint32_t d2c_3c8e0(uint32_t, const cl::Env& e) { return clc::belt(e, 3, 0x11C487); }
