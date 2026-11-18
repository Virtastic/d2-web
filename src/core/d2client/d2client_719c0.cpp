// 1.13c D2Client+0x719C0 -- mouse-down on the third panel button (stack = message, tail jump to Ordinal 511).
// g (one dword per global, the 0x718E0 layout): +0 [11C00C], +4 [11C010], +0xC [11B9A4], +0x10 [11B828] x, +0x14 [11B9A0],
// +0x18 [11B824] y, +0x24 [11C3DC], +0x28 [11C3E0], +0x2C [11C3F8], +0x30 [11C3FC].
// A press with x in [B9A0+0x110, +0x60] and y in [0x136-B9A4-0x20, 0x136-B9A4] (signed) sets [11C010] = 1, [11C00C] = 1 and calls
// 0x88A70(0,0,0,0) with EBX = 4; a miss clears [11C3DC], [11C3E0], [11C3F8], [11C3FC]. Message +0x1C = 0, +0x18 = 1, Ordinal 511.
#include "cl_env.h"
extern "C" void d2c_719c0(uint32_t msg, const cl::Env& e, uint32_t g) {
    int32_t x = (int32_t)e.R(g + 0x10), y = (int32_t)e.R(g + 0x18);
    int32_t top = 0x136 - (int32_t)e.R(g + 0xc);
    int32_t lo = (int32_t)e.R(g + 0x14) + 0x110;
    if (x >= lo && x <= lo + 0x60 && y >= top - 0x20 && y <= top) {
        e.W(g + 4, 1); e.W(g, 1);
        e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 4);
    } else {
        e.W(g + 0x24, 0); e.W(g + 0x28, 0); e.W(g + 0x2c, 0); e.W(g + 0x30, 0);
    }
    e.W(msg + 0x1c, 0); e.W(msg + 0x18, 1); e.H(0xbe84, msg);
}
