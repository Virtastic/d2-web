// 1.13c D2Client+0x3C960 -- key command: belt slot 1 (flag byte 0x11C485).
#include "cl_cmd.h"
extern "C" uint32_t d2c_3c960(uint32_t, const cl::Env& e) { return clc::belt(e, 1, 0x11C485); }
