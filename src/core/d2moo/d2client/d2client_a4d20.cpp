// 1.13c D2Client+0xA4D20 -- fastcall slot of the callback table at 0xDE838 (ECX unused, EDX = the argument): pushes EDX and runs the 0xC5F8
// import thunk; its answer is the answer.
#include "cl_pa4.h"
extern "C" uint32_t d2c_a4d20(uint32_t, uint32_t edx, const cl::Env& e) { return e.H(0xc5f8, edx); }
