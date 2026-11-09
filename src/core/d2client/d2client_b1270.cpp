// 1.13c D2Client+0xb1270 -- client missile spawn notify: find what the missile hits around it. EAX = unit, returns 1 / 0.
// No unit / bad class: 1. 0xC322(unit) & 4: 1. No owner (0xA60D0(EAX = unit)): 1. y = 0x1660(ECX = unit), x = 0x1630(ECX = unit);
// a stack record L[6] = {0, -1, 0, -1, -1, 0}; 0xA1860(owner, x, y, row +0x3C, 0xA283, 0xA09F0, &L, 0) (ret 0x20) fills it;
// 0xB02E0(EAX = L[0] ? L[0] : L[2], ESI = unit); returns 0.
#include "cl_env.h"
extern "C" uint32_t d2c_b1270(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row) return 1;
    uint32_t r3c = e.R(row + 0x3c);
    if (e.H(0xc322, u) & 4) return 1;
    uint32_t own = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!own) return 1;
    uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
    uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
    uint32_t L[6] = { 0, 0xffffffffu, 0, 0xffffffffu, 0xffffffffu, 0 };
    e.Xs(0xB2000000u | 0xa09f0u, (uint32_t)(uintptr_t)L);
    e.H(0xa1860, own, x, y, r3c, 0xa283);
    e.H(0xb02e0, 0, 0, 0, 0, 0, L[0] ? L[0] : L[2], 0, 0, 0, u);
    return 0;
}
