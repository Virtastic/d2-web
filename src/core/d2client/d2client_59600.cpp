// 1.13c D2Client+0x59600 -- exit-confirm send O/04. gl layout (module slot RVA): +0x00 0x1177E8, +0x04 0x1177EC (halves at 0x1177EC/EE),
// +0x08 0x1177F0, +0x0C 0x1177F4 stamp, +0x10 0x1177F8, +0x14 0x11C2CC state.
// State := 7, send {'O', 4, 0, w[0x1177EE], w[0x1177EC]} (0x143E0, stack buffer, EBX = 7), stamp := -1.
#include "cl_env.h"
extern "C" void d2c_59600(const cl::Env& e) {
    const uint32_t g = e.gl;
    e.W(g + 0x14, 7);
    const uint32_t w = e.R(g + 4);
    uint8_t p[8] = {'O', 4, 0, (uint8_t)(w >> 16), (uint8_t)(w >> 24), (uint8_t)w, (uint8_t)(w >> 8), 0};
    e.H(0x143e0, (uint32_t)(uintptr_t)p, 0, 0, 0, 0, 0, 0, 0, 7, 0);
    e.W(g + 0xc, 0xFFFFFFFFu);
}
