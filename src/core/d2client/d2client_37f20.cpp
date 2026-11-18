// 1.13c D2Client+0x37F20 -- method (ECX = object, one stack word): stores the word at +0x24 and leaves it in EAX.
#include "cl_tbl.h"
extern "C" uint32_t d2c_37f20(uint32_t self, uint32_t a, const cl::Env& e) { e.W(self + 0x24, a); return a; }
