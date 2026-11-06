// 1.13c D2Client+0x59740 -- exit-confirm close (stack param_1). gl layout (module slot RVA): +0x00 0x1177E8 box B, +0x08 0x1177F0 box A,
// +0x14 0x11C2CC state, +0x18 0x11C2D0, +0x1C 0x11BBFC player, +0x20 0xFADDC in-game, +0x24 0xFAD94, +0x28 0x11BE74.
// c2d0 := 0; each non-zero box: 0xA3D20, slot := 0 (A then B). State 7: done. In game: 0xC2790(0x17, ECX 1, EDX 0), be74 := 0, 0x8BB90;
// player with unit+0x10 != 0x11 and param_1: 0xC2790(1, 2, 0); fad94: 0xC2790(5, 1, 0). Then a non-zero state sends {'O', 2, 0, 0, 0, 0, 0}
// (0x143E0, EBX = 7) and clears the state.
#include "cl_env.h"
extern "C" void d2c_59740(uint32_t param_1, const cl::Env& e) {
    const uint32_t g = e.gl;
    e.W(g + 0x18, 0);
    if (e.R(g + 8)) { e.H(0xa3d20); e.W(g + 8, 0); }
    if (e.R(g + 0)) { e.H(0xa3d20); e.W(g + 0, 0); }
    if (e.R(g + 0x14) == 7) return;
    if (e.R(g + 0x20)) {
        e.H(0xc2790, 0x17, 0, 0, 0, 0, 0, 1, 0);
        e.W(g + 0x28, 0);
        e.H(0x8bb90);
        const uint32_t u = e.R(g + 0x1c);
        if (u && e.R(u + 0x10) != 0x11 && param_1) e.H(0xc2790, 1, 0, 0, 0, 0, 0, 2, 0);
        if (e.R(g + 0x24)) e.H(0xc2790, 5, 0, 0, 0, 0, 0, 1, 0);
    }
    if (e.R(g + 0x14)) {
        const uint8_t p[8] = {'O', 2, 0, 0, 0, 0, 0, 0};
        e.H(0x143e0, (uint32_t)(uintptr_t)p, 0, 0, 0, 0, 0, 0, 0, 7, 0);
        e.W(g + 0x14, 0);
    }
}
