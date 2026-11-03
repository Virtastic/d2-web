// 1.13c D2Client+0x71130: two-pass cel draw. stdcall(a0, a1): both passes draw through the D2Gfx #10041 thunk (D2Client+0xD096,
// stdcall 6) with one 0x48-byte record, +0 = a0 then a1, +0x34 = DAT 0x11C004. The retail code reads a1 after the first call.
#include "d2client_callee.h"
extern "C" void d2c_71130_svc(const D2cSvc& v, uint32_t a0, uint32_t a1, uint32_t col /*0x11C004*/, int32_t px /*0x11B9A0*/, int32_t py /*0x11B9A4*/) {
    uint32_t blk[18] = {};
    blk[0] = a0; blk[13] = col;
    d2c_call(v, 0xd096, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)blk, (uint32_t)(px + 0xce), (uint32_t)(0x13e - py), 0xffffffffu, 5, 0);
    blk[0] = a1;
    d2c_call(v, 0xd096, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)blk, (uint32_t)(px + 0x152), (uint32_t)(0x13e - py), 0xffffffffu, 5, 0);
}
