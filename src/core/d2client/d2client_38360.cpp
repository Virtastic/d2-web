// 1.13c D2Client+0x38360: edit-box text in (thiscall, ECX = this, one stack word = the source char string, ret 4).
// 0xCEAC0 (D2Lang win2Unicode: ECX = this+0x28 buffer, EDX = source, stack word = capacity this+0x34 as 16 bits) widens it,
// 0xCEAB0 (ECX = this+0x28) measures it; the length goes to this+0x36 (cursor) and this+0x3A (length). Returns the length.
#include "cl_env.h"
extern "C" uint32_t d2c_38360(uint32_t self, uint32_t src, const cl::Env& e) {
    const uint32_t buf = e.R(self + 0x28), cap = e.R(self + 0x34) & 0xffff;
    e.H(0xceac0, cap, 0, 0, 0, 0, 0, buf, src);
    const uint32_t n = e.H(0xceab0, 0, 0, 0, 0, 0, 0, buf);
    e.W(self + 0x36, n);
    e.W(self + 0x3a, n);
    return n;
}
