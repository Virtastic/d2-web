// 1.13c D2Client+0xb83d0 -- client missile: a child in a random box around the unit on an interval. ECX = unit, no return.
// No unit / bad class / row +0x1E < 0: 0xB1720(ESI = unit). 0xC4D2(unit) % max(row +0x60, 1) != 0: the 0xB17C0 tail.
// Otherwise (0x1380 = ECX seed, ESI range) a, b = rnd(2*row58+1) - row58; cx = rnd(2*row5C+1) - row5C + 0x1630(unit);
// cy = rnd(...) - row5C + 0x1660(unit). Block word 0 = 0x21, 1 = 0xA60D0(EAX = unit), 5/6 = cx/cy, 7/8 = cx+a / cy+b,
// 11 = 0xC388(unit), 12 = 0xC3FA(unit), 4 = row +0x1E; 0xB30F0(ECX = block); then the 0xB17C0(ECX = unit) tail.
#include "cl_env.h"
extern "C" void d2c_b83d0(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x1e) < 0) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    int32_t n = (int32_t)e.R(row + 0x60); if (n < 1) n = 1;
    if ((int32_t)e.H(0xc4d2, u) % n != 0) { e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u); return; }
    uint32_t s = u + 0x20;
    uint32_t r58 = e.R(row + 0x58);
    uint32_t a = e.H(0x1380, 0, 0, 0, 0, 0, 0, s, 0, 0, r58 + r58 + 1) - r58;
    uint32_t b = e.H(0x1380, 0, 0, 0, 0, 0, 0, s, 0, 0, r58 + r58 + 1) - e.R(row + 0x58);
    uint32_t r5c = e.R(row + 0x5c);
    uint32_t cx = e.H(0x1380, 0, 0, 0, 0, 0, 0, s, 0, 0, r5c + r5c + 1) - r5c;
    cx += e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
    uint32_t cy = e.H(0x1380, 0, 0, 0, 0, 0, 0, s, 0, 0, r5c + r5c + 1) - e.R(row + 0x5c);
    cy += e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
    uint32_t q[23] = {};
    q[0] = 0x21;
    q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    q[5] = cx; q[6] = cy; q[7] = cx + a; q[8] = cy + b;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[4] = (uint32_t)e.R16s(row + 0x1e);
    e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
