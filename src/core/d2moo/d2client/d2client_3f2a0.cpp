// 1.13c D2Client+0x3F2A0 -- two-button hit test on a control (ECX = control, stack = mouse message; thiscall, ret 4).
// Control: +0x10/+0x14 first button's left/bottom, +0x18/+0x1C second button's left/bottom, +0x24 button height, +0x28 width,
// +0x30 "pressed seen" flag, +0x34/+0x38 per-button pressed flags, +0x3C callback, +0x44 press tick, +0x4C busy flag.
// Message: u16 x at +0xC, u16 y at +0xE. The comparisons are signed, strict on left/right, strict-low/inclusive-high on
// the first button's y range and strict on both ends of the second's.
#include "cl_env.h"
extern "C" uint32_t d2c_3f2a0(uint32_t self, uint32_t msg, const cl::Env& e, uint32_t now) {
    e.W(self + 0x30, 1);
    int32_t x = (int32_t)(e.R(msg + 0xc) & 0xffff), y = (int32_t)((e.R(msg + 0xc) >> 16) & 0xffff);
    int32_t l0 = (int32_t)e.R(self + 0x10), w = (int32_t)e.R(self + 0x28);
    int32_t b0 = (int32_t)e.R(self + 0x14), h = (int32_t)e.R(self + 0x24);
    if (x > l0 && x < w + l0 && y > b0 - h && y <= b0) {
        e.W(self + 0x44, now); e.W(self + 0x34, 1); e.W(self + 0x4c, 1);
        e.H(e.R(self + 0x3c) - 0x6FAB0000u, 0, 0, 0, 0, 0, 0, 0, 0, 0, self);
        e.W(self + 0x4c, 0);
        return 1;
    }
    int32_t l1 = (int32_t)e.R(self + 0x18), b1 = (int32_t)e.R(self + 0x1c);
    if (x > l1 && x < w + l1 && y > b1 - h && y < b1) {
        e.W(self + 0x44, now); e.W(self + 0x38, 1); e.W(self + 0x4c, 1);
        e.H(e.R(self + 0x3c) - 0x6FAB0000u, 0, 0, 0, 0, 0, 0, 0, 0, 0, self);
        e.W(self + 0x4c, 0);
        return 1;
    }
    return 0;
}
