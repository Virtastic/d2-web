// 1.13c D2Client+0x71220: one cel draw, stdcall(a0): record +0 = a0, +0x34 = DAT 0x11C004, through D2Gfx #10041 (D2Client+0xD096).
#include "d2client_callee.h"
extern "C" void d2c_71220_svc(const D2cSvc& v, uint32_t a0, uint32_t col /*0x11C004*/, int32_t px /*0x11B9A0*/, int32_t py /*0x11B9A4*/) {
    uint32_t blk[18] = {};
    blk[0] = a0; blk[13] = col;
    d2c_call(v, 0xd096, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)blk, (uint32_t)(px + 0x110), (uint32_t)(0x13e - py), 0xffffffffu, 5, 0);
}
