// 1.13c D2Client+0x383F0 -- method (ECX = object, one stack word): stores the word at +0x20 and leaves it in EAX.
#include "cl_tbl.h"
extern "C" uint32_t d2c_383f0(uint32_t self, uint32_t a, const cl::Env& e) { e.W(self + 0x20, a); return a; }
