// 1.13c D2Client+0x382F0: edit-box text out (thiscall, ECX = this, one stack word = the destination char buffer, ret 4).
// Converts the box's wide text (this+0x28) to UTF with the D2Lang routine in import slot 0xCEAE8 (ECX = dest, EDX = this+0x28 text,
// stack word = the capacity this+0x34 as a 16-bit value); its result is returned.
#include "cl_env.h"
extern "C" uint32_t d2c_382f0(uint32_t self, uint32_t dest, const cl::Env& e) {
    const uint32_t cap = e.R(self + 0x34) & 0xffff;
    return e.H(0xceae8, cap, 0, 0, 0, 0, 0, dest, e.R(self + 0x28));
}
