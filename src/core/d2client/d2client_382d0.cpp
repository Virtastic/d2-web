// 1.13c D2Client+0x382D0 -- method (ECX = object, one unused stack word): sets the dword at +0x3E to 1. Returns 0.
#include "cl_tbl.h"
extern "C" uint32_t d2c_382d0(uint32_t self, const cl::Env& e) { e.W(self + 0x3e, 1); return 0; }
