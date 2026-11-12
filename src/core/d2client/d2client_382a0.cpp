// 1.13c D2Client+0x382A0 -- method (ECX = object, one unused stack word): when the dword at +0x3E is set, the dwords at
// +0x46 and +0x3E are cleared. Returns 0.
#include "cl_tbl.h"
extern "C" uint32_t d2c_382a0(uint32_t self, const cl::Env& e) {
    if (e.R(self + 0x3e)) { e.W(self + 0x46, 0); e.W(self + 0x3e, 0); }
    return 0;
}
