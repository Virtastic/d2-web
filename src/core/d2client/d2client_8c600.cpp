// 1.13c D2Client+0x8C600: AL = w, CL = h, EDX = x0, ESI = cell geometry, stack y0, colour index (ret 8).
// For each column ox in [x0, x0+w) and row iy in [y0, y0+h): px = cell[+0x14] * iy + cell[+4], py = cell[+0x15] * ox + cell[+0xC];
// a rectangle inside the screen (D2Client+0xF9E14 x +0xF9E18) is filled through D2Gfx #10014 (thunk 0xD0BA, stdcall 6):
// (px, py, px + cell[+0x14], py + cell[+0x15], palette byte D2Client+0x10B330[ci], 0).
#include "cl_env.h"
extern "C" void d2cl_8c600(uint32_t w, uint32_t h, uint32_t x0, uint32_t cell, uint32_t y0, uint32_t ci, const cl::Env& e) {
    const uint32_t xe = x0 + (w & 0xff), ye = y0 + (h & 0xff);
    for (uint32_t ox = x0; (int32_t)ox < (int32_t)xe; ++ox) {
        for (uint32_t iy = y0; (int32_t)iy < (int32_t)ye; ++iy) {
            const uint32_t sx = e.R8(cell + 0x14), sy = e.R8(cell + 0x15);
            const uint32_t px = sx * iy + e.R(cell + 4), py = sy * ox + e.R(cell + 0xc);
            if ((int32_t)px < 0 || (int32_t)px >= (int32_t)e.R(0x6FAB0000u + 0xF9E14u)) continue;
            if ((int32_t)py < 0 || (int32_t)py >= (int32_t)e.R(0x6FAB0000u + 0xF9E18u)) continue;
            const uint32_t a[6] = { px, py, px + sx, py + sy, e.R8(0x6FAB0000u + 0x10B330u + ci), 0 };
            e.H(0xd0ba, a[0], a[1], a[2], a[3], a[4]);
            if (e.blk) e.blk(e.ctx, 0xd0ba, (uint32_t)(uintptr_t)a, 6);
        }
    }
}
