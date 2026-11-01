// 1.13c D2Client+0x200E0 -- unit effect 0x2A at its position: fastcall ECX = unit, one unused stack word (ret 4).
// 10569(unit, &kind, &other); a live unit (+0x10 == 0) whose kind is 4 calls 0xB0450 (ESI = unit; stack 0x2A, x, y, 0, 1),
// y read before x (path block for types 2/4/5, else the D2Common getters on the path).
#include "cl_env.h"
extern "C" void d2c_200e0(uint32_t u, const cl::Env& e) {
    uint32_t k[2] = {0, 0};
    e.H(0xc5ec, u, (uint32_t)(uintptr_t)&k[0], (uint32_t)(uintptr_t)&k[1]);
    if (u && e.R(u + 0x10)) return;
    if (k[0] != 4) return;
    uint32_t y = e.PD(u, 0xc2ec, 0x10), x = e.PD(u, 0xc3be, 0xc);
    e.H(0xb0450, 0x2a, x, y, 0, 1, 0, 0, 0, 0, u);
}
