// 1.13c D2Client+0xb6f60 -- client missile: counted spawn at the owner's target. ECX = unit, no return.
// No unit / bad class / row +0x1E < 0, or no owner (0xA60D0(EAX = unit)): 0xB1720(ESI = unit). 0xB2890(ECX = unit);
// 0xA1340(EBX = owner, EDI = &x, stack &y) == 0: return (no tail). t = 0xC4BA(unit), c = 0xC322(unit); t < c: 0xC394(unit, t+1).
// Block word 0 = 0x20, 1 = 2 = owner, 7/8 = x/y, 11 = 0xC388(unit), 12 = 0xC3FA(unit), 4 = row +0x1E; m = 0xB30F0(ECX = block);
// m: 0xC38E(m path, 0xE), 0xC376(m path, m, 0). Then 0xC394(unit, 0) and the 0xB17C0(ECX = unit) tail.
#include "cl_env.h"
extern "C" void d2c_b6f60(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x1e) < 0) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    uint32_t own = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!own) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    e.H(0xb2890, 0, 0, 0, 0, 0, 0, u);
    uint32_t x = 0, y = 0;
    e.Ed((uint32_t)(uintptr_t)&x);
    if (!e.H(0xa1340, (uint32_t)(uintptr_t)&y, 0, 0, 0, 0, 0, 0, 0, own)) return;
    int32_t t = (int32_t)e.H(0xc4ba, u);
    int32_t c = (int32_t)e.H(0xc322, u);
    if (t < c) e.H(0xc394, u, (uint32_t)t + 1u);
    uint32_t q[23] = {};
    q[0] = 0x20; q[1] = own; q[2] = own; q[7] = x; q[8] = y;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[4] = (uint32_t)e.R16s(row + 0x1e);
    uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (m) {
        e.H(0xc38e, e.R(m + 0x2c), 0xe);
        e.H(0xc376, e.R(m + 0x2c), m, 0);
    }
    e.H(0xc394, u, 0);
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
