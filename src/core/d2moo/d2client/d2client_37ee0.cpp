// 1.13c D2Client+0x37EE0 -- method (ECX = object, one unused stack word): the dword at +0x36, when above zero (signed),
// goes down by one. Returns 1.
#include "cl_tbl.h"
extern "C" uint32_t d2c_37ee0(uint32_t self, const cl::Env& e) {
    const int32_t v = (int32_t)e.R(self + 0x36);
    if (v > 0) e.W(self + 0x36, (uint32_t)v - 1);
    return 1;
}
