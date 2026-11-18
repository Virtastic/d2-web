// 1.13c D2Client+0x3A690 -- key command: one callee with ECX = 1.
#include "cl_cmd.h"
extern "C" uint32_t d2c_3a690(uint32_t, const cl::Env& e) { return e.H(0x608d0, 0, 0, 0, 0, 0, 0, 1); }
