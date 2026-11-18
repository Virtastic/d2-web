// 1.13c D2Client+0x37F30 -- method (ECX = object, one stack word): stores the word at +0x18 and leaves it in EAX.
#include "cl_tbl.h"
extern "C" uint32_t d2c_37f30(uint32_t self, uint32_t a, const cl::Env& e) { e.W(self + 0x18, a); return a; }
