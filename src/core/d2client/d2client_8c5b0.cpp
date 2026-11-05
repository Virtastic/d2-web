// 1.13c D2Client+0x8C5B0: EDI = colour mode, EDX = point record. Mode 2 draws nothing; a point
// (x at +0, y at +8) inside [0, W) x [0, H) (D2Client+0xF9E14 / +0xF9E18) goes to 0x68970 (EAX = y, ECX = x, EDX = 0; stack: the
// bytes at +0x10 and +0x11, then the palette byte D2Client+0x10B330[mode]).
#include "cl_env.h"
extern "C" void d2cl_8c5b0(uint32_t mode, uint32_t pt, const cl::Env& e) {
    if (mode == 2) return;
    const int32_t x = (int32_t)e.R(pt), y = (int32_t)e.R(pt + 8);
    if (x < 0 || x >= (int32_t)e.R(0x6FAB0000u + 0xF9E14u) || y < 0 || y >= (int32_t)e.R(0x6FAB0000u + 0xF9E18u)) return;
    const uint32_t col = e.R8(0x6FAB0000u + 0x10B330u + mode);
    e.H(0x68970, e.R8(pt + 0x10), e.R8(pt + 0x11), col, 0, 0, (uint32_t)y, (uint32_t)x, 0);
}
