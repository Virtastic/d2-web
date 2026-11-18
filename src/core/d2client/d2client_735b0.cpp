// 1.13c D2Client+0x735B0 -- refresh a player's portal skill state. ECX = unit, EDX = skill (plain ret).
// EDI (8 or 1, passed to 0x2A6C0) is not observable through a recorder and is not modelled.
#include "cl_env.h"
extern "C" void d2c_735b0(uint32_t u /*ECX*/, uint32_t skill /*EDX*/, const cl::Env& e) {
    if (!u || e.R(u) != 1) return;
    uint32_t h = e.H(0xc1c0, u, skill);
    if (!h) return;
    uint32_t v = e.H(0xc4b4, h, 0x163, 0);
    e.H(0x4d610, v, 0, 0, 0, 0, u);
    e.H(0x2a6c0, 0, 0, 0, 0, 0, 0, 0, 0, 0, u);
    if (v == 0x21f) {
        uint32_t p = e.R(u + 0x2c);
        uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, u) + 10;
        uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
        e.H(0xc478, p, x, y, 1);
    }
}
