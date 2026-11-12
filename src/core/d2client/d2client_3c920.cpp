// 1.13c D2Client+0x3C920 -- key command: belt slot 2 (flag byte 0x11C486).
#include "cl_cmd.h"
extern "C" uint32_t d2c_3c920(uint32_t, const cl::Env& e) { return clc::belt(e, 2, 0x11C486); }
