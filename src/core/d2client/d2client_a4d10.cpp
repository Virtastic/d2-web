// 1.13c D2Client+0xA4D10 -- fastcall slot of the callback table at 0xDE838 (ECX unused, EDX = the argument): runs the 0xC5E0 import thunk with
// (EDX, 0); its answer is the answer.
#include "cl_pa4.h"
extern "C" uint32_t d2c_a4d10(uint32_t, uint32_t edx, const cl::Env& e) { return e.H(0xc5e0, edx, 0); }
