// 1.13c D2Client+0x59650 -- exit-confirm send O/02 and arm the wait. gl layout as 0x59600 (+0x0C stamp 0x1177F4, +0x10 flag 0x1177F8,
// +0x14 state 0x11C2CC). Send {'O', 2, 0, 0, 0, 0, 0} (0x143E0, EBX = 7); unless the state is 7: flag := 1, stamp := GetTickCount (tick), state := 7.
#include "cl_env.h"
extern "C" void d2c_59650(uint32_t tick, const cl::Env& e) {
    const uint32_t g = e.gl;
    const uint8_t p[8] = {'O', 2, 0, 0, 0, 0, 0, 0};
    e.H(0x143e0, (uint32_t)(uintptr_t)p, 0, 0, 0, 0, 0, 0, 0, 7, 0);
    if (e.R(g + 0x14) != 7) {
        e.W(g + 0x10, (e.R(g + 0x10) & 0xFFFFFF00u) | 1u);   // retail: mov BYTE [flag], 1 (the upper three bytes stay)
        e.W(g + 0xc, tick);
        e.W(g + 0x14, 7);
    }
}
