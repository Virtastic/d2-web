// 1.13c D2Client+0x3C9A0 -- key command: belt slot 0 (flag byte 0x11C484).
#include "cl_cmd.h"
extern "C" uint32_t d2c_3c9a0(uint32_t, const cl::Env& e) { return clc::belt(e, 0, 0x11C484); }
