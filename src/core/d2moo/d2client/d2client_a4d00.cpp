// 1.13c D2Client+0xA4D00 -- fastcall slot of the callback table at 0xDE838 (ECX unused, EDX = the argument): pushes EDX and runs the 0xC6B2
// import thunk; its answer is the answer.
#include "cl_pa4.h"
extern "C" uint32_t d2c_a4d00(uint32_t, uint32_t edx, const cl::Env& e) { return e.H(0xc6b2, edx); }
