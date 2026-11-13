// 1.13c D2Client+0x72030 -- state callback (ECX = unit): the two-word unit routine with (unit, 0).
#include "cl_pa4.h"
extern "C" uint32_t d2c_72030(uint32_t ecx, const cl::Env& e) { return e.H(0xc358, ecx, 0); }
