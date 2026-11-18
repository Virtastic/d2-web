// 1.13c D2Client+0x718E0 -- mouse-down on the two-button panel (stack = message, tail jump to Ordinal 511).
// g = pinned globals, one dword each (the 0x71400 layout): +0 [11C00C], +4 [11C010], +8 [11C014], +0xC [11B9A4], +0x10 [11B828] x,
// +0x14 [11B9A0], +0x18 [11B824] y, +0x24 [11C3DC], +0x28 [11C3E0], +0x2C [11C3F8], +0x30 [11C3FC].
// A press in the first button box (x in [B9A0+0xCE, +0x60], y in [0x136-B9A4-0x20, 0x136-B9A4], signed) sets [11C010] = 1, in the
// second (from B9A0+0x152) [11C014] = 1; either then sets [11C00C] = 1 and calls 0x88A70(0,0,0,0) with EBX = 4. A miss clears
// [11C3DC], [11C3E0], [11C3F8], [11C3FC]. Message +0x1C = 0, +0x18 = 1, Ordinal 511.
#include "cl_env.h"
extern "C" void d2c_718e0(uint32_t msg, const cl::Env& e, uint32_t g) {
    int32_t base = (int32_t)e.R(g + 0x14), x = (int32_t)e.R(g + 0x10), y = (int32_t)e.R(g + 0x18);
    int32_t top = 0x136 - (int32_t)e.R(g + 0xc);
    int32_t lo = base + 0xce;
    bool hit = false;
    if (x >= lo && x <= lo + 0x60 && y >= top - 0x20 && y <= top) { e.W(g + 4, 1); hit = true; }
    else {
        lo = base + 0x152;
        if (x >= lo && x <= lo + 0x60 && y >= top - 0x20 && y <= top) { e.W(g + 8, 1); hit = true; }
    }
    if (hit) {
        e.W(g, 1);
        e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 4);
    } else {
        e.W(g + 0x24, 0); e.W(g + 0x28, 0); e.W(g + 0x2c, 0); e.W(g + 0x30, 0);
    }
    e.W(msg + 0x1c, 0); e.W(msg + 0x18, 1); e.H(0xbe84, msg);
}
