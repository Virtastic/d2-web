// 1.13c D2Client+0xC0680 (ecx = unit): with the UI mode (DAT_6FBCC414) not 3 and the unit not the local player (e.self), reads its screen position through 0xC21A(unit, &{x,y}), makes it relative to the panel rect (e.gl = *DAT_6FBCC3C4: left +0x24, top +0x28; only the SetRect call sees the rect, its result is unused), applies the mode 1 / 2 quarter-width shift and rejects what is off the 0xDBC48 x 0xDBC4C window. An accepted unit gets 128 default Unicode constructions (D2Lang ??_FUnicode@@QAEXXZ, ECX = element) and 0xC0450(EAX = &unit-copy, ECX = unit, stack &mode-copy, &buffer, 0x80).
// e.gl: the panel struct; e.gl2: 3 dwords = the UI mode (0x11C414), the width (0xDBC48), the height (0xDBC4C).
#include "cl_env.h"
extern "C" void d2c_c0680(uint32_t u, const cl::Env& e) {
    uint32_t su = u;
    uint32_t sm = e.R(e.gl2);
    if (sm == 3 || u == e.self) return;
    const int32_t g = (int32_t)e.gl;
    const int32_t left = (int32_t)e.R(g + 0x24), top = (int32_t)e.R(g + 0x28);
    uint32_t pos[2] = {0, 0};
    e.H(0xc21a, u, (uint32_t)(uintptr_t)pos);
    const int32_t W = (int32_t)e.R(e.gl2 + 4), H = (int32_t)e.R(e.gl2 + 8);
    int32_t x = (int32_t)pos[0] - left;
    const int32_t y = (int32_t)pos[1] + (-0x46 - top);
    const int32_t half = (W - (W >> 31)) >> 1;
    const int32_t quarter = (W + ((W >> 31) & 3)) >> 2;
    if (sm == 1) { x -= quarter; if (x > half) return; }
    else if (sm == 2) { x += quarter; if (x < half) return; }
    if (y <= 0 || y >= W || x <= -0x32 || x >= H + 0x32) return;
    uint16_t buf[128];
    for (int i = 0; i < 128; i++) { buf[i] = 0; e.H(0xceb00, 0, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)&buf[i]); }
    e.H(0xc0450, (uint32_t)(uintptr_t)&sm, (uint32_t)(uintptr_t)buf, 0x80, 0, 0, (uint32_t)(uintptr_t)&su, u);
}
