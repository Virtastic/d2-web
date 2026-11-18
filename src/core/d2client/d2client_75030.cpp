// 1.13c D2Client+0x75030: EAX = y; two D2Gfx #10041 draws via 0xD096 with an 18-dword block.
#include "cl_env.h"
extern "C" void d2c_75030(uint32_t y, const cl::Env& e) {
    const uint32_t B = 0x6FAB0000u;
    uint32_t blk[18] = {};
    blk[13] = e.R(B + 0x11bfec);
    uint32_t bp = (uint32_t)(uintptr_t)blk, px = e.R(B + 0x11b9a0), yy = y - e.R(B + 0x11b9a4);
    e.H(0xd096, bp, px + 0xc, yy, 0xffffffffu, 5, 0);
    blk[0] = 1;
    e.H(0xd096, bp, px + 0x10b, yy, 0xffffffffu, 5, 0);
}
