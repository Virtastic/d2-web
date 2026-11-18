// 1.13c D2Client+0x44730 -- mouse-down on the screen edge scroll zone (stack = message, ret 4). g = pinned globals:
// +0 gate [0x11C3A0], +4 mode [0x11C414], +8 W [0xDBC48], +0xC H [0xDBC4C], +0x10 [0x11C3E0], +0x14 [0x11C3E4],
// +0x18 [0x11C2F8], +0x1C [0xF46C8], +0x20 playerSelf [0x11BBFC],
// +0x24, +0x28, +0x2C [0x11C3E8], [0x11C3EC], [0x11C3F0] (read by the inlined 0x427C0).
// Always sets [F46C8]=4 and clears [11C3E4], [11C2F8]; with the gate clear or mode 3 it returns there. Otherwise the click is a
// miss (message +0x18 = +0x1C = 0, Storm 511) when mode 2 and the point is inside the left half above the bottom strip, or
// when 0x426D0 answers nonzero; else [11C3E0]=1 and, with a player whose [0] is 0, [11C3E0] = 0x1BF20(3, x, y, 0x427C0()),
// then message +0x18 = 1, +0x1C = 0, Storm 511.
#include "cl_env.h"
extern "C" void d2c_44730(uint32_t msg, const cl::Env& e, uint32_t g) {
    e.W(g + 0x1c, 4); e.W(g + 0x14, 0); e.W(g + 0x18, 0);
    if (e.R(g) == 0) return;
    int32_t mode = (int32_t)e.R(g + 4);
    if (mode == 3) return;
    int32_t y = (int32_t)((e.R(msg + 0xc) >> 16) & 0xffff), x = (int32_t)(e.R(msg + 0xc) & 0xffff);
    bool miss = false;
    if (mode == 2) {
        int32_t w = (int32_t)e.R(g + 8);
        int32_t half = (w - (w >> 31)) >> 1;
        if (x > 0 && x < half && y > 0 && y < (int32_t)e.R(g + 0xc) - 0x2f) miss = true;
    }
    if (!miss && e.H(0x426d0) != 0) miss = true;
    if (miss) {
        e.W(msg + 0x1c, 0); e.W(msg + 0x18, 0); e.H(0xbe84, msg);
        return;
    }
    e.W(g + 0x10, 1);
    // 0x427C0 is a pure accessor of [11C3E8], [11C3EC], [11C3F0] (g +0x24, +0x28, +0x2C), kept inline so EDX (the player) survives it as in retail
    uint32_t a = (e.R(g + 0x24) ? 8u : 0u);
    if (e.R(g + 0x28)) a |= 8u;
    if (e.R(g + 0x2c)) a |= 4u;
    uint32_t pl = e.R(g + 0x20);
    if (pl != 0 && e.R(pl) == 0) e.W(g + 0x10, e.H(0x1bf20, 3, (uint32_t)x, (uint32_t)y, a));
    e.W(msg + 0x1c, 0); e.W(msg + 0x18, 1); e.H(0xbe84, msg);
}
