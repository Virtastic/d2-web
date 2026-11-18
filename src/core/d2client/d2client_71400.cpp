// 1.13c D2Client+0x71400 -- mouse-up on the two-button panel (stack = message, ends in a tail jump to Ordinal 511 which pops it).
// g = pinned globals, one dword each: +0 [11C00C] armed, +4 [11C010], +8 [11C014], +0xC [11B9A4], +0x10 [11B828] mouse x,
// +0x14 [11B9A0], +0x18 [11B824] mouse y, +0x1C [11C01C], +0x20 [11C020], +0x24 [11C3DC], +0x28 [11C3E0], +0x2C [11C3F8], +0x30 [11C3FC].
// Clears [11C010] and [11C014]; with [11C00C] clear it clears [11C3DC], [11C3E0], [11C3F8], [11C3FC]. Otherwise a release inside
// the first button box (x in [B9A0+0xCE, +0x60], y in [0x136-B9A4-0x20, 0x136-B9A4], all signed) sets [11C01C] = 1 and clears
// [11C00C]; the second box (x from B9A0+0x152) sets [11C020] = 1 and clears [11C00C]. Message +0x1C = 0, +0x18 = 1, Ordinal 511.
#include "cl_env.h"
extern "C" void d2c_71400(uint32_t msg, const cl::Env& e, uint32_t g) {
    e.W(g + 4, 0); e.W(g + 8, 0);
    if (e.R(g) == 0) {
        e.W(g + 0x24, 0); e.W(g + 0x28, 0); e.W(g + 0x2c, 0); e.W(g + 0x30, 0);
        e.W(msg + 0x1c, 0); e.W(msg + 0x18, 1); e.H(0xbe84, msg);
        return;
    }
    int32_t base = (int32_t)e.R(g + 0x14), x = (int32_t)e.R(g + 0x10), y = (int32_t)e.R(g + 0x18);
    int32_t top = 0x136 - (int32_t)e.R(g + 0xc);
    int32_t lo = base + 0xce;
    if (x >= lo && x <= lo + 0x60 && y >= top - 0x20 && y <= top) { e.W(g + 0x1c, 1); e.W(g, 0); }
    lo = base + 0x152;
    if (x >= lo && x <= lo + 0x60 && y >= top - 0x20 && y <= top) { e.W(g + 0x20, 1); e.W(g, 0); }
    e.W(msg + 0x1c, 0); e.W(msg + 0x18, 1); e.H(0xbe84, msg);
}
