// 1.13c D2Client+0xb79f0 -- client missile: trail of random-offset children on an interval. ECX = unit, no return.
// No unit / bad class / row +0x1E < 0: 0xB1720(ESI = unit). t = 0xC4D2(unit); t == row +0x58: 0xC394(unit, 1).
// When 0xC4BA(unit) != 0 and t % max(row +0x5C, 1) == 0: n = row +0x60, rx = 0x1380(ECX = seed, ESI = 2n+1) - n, ry the same;
// block word 0 = 0x21, 1 = 0xA60D0(EAX = unit), 5/6 = 0x1630/0x1660(ECX = unit), 7/8 = the same plus rx/ry, 11 = 0xC388(unit),
// 12 = 0xC3FA(unit), 4 = row +0x1E; 0xB30F0(ECX = block). Then the 0xB17C0(ECX = unit) tail.
#include "cl_env.h"
extern "C" void d2c_b79f0(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x1e) < 0) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    int32_t t = (int32_t)e.H(0xc4d2, u);
    if (t == (int32_t)e.R(row + 0x58)) e.H(0xc394, u, 1);
    if (e.H(0xc4ba, u)) {
        int32_t m = (int32_t)e.R(row + 0x5c); if (m < 1) m = 1;
        if (t % m == 0) {
            uint32_t n = e.R(row + 0x60);
            uint32_t rx = e.H(0x1380, 0, 0, 0, 0, 0, 0, u + 0x20, 0, 0, n + n + 1) - n;
            uint32_t ry = e.H(0x1380, 0, 0, 0, 0, 0, 0, u + 0x20, 0, 0, n + n + 1) - n;
            uint32_t q[23] = {};
            q[0] = 0x21;
            q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
            q[5] = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
            q[6] = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
            q[7] = e.H(0x1630, 0, 0, 0, 0, 0, 0, u) + rx;
            q[8] = e.H(0x1660, 0, 0, 0, 0, 0, 0, u) + ry;
            q[11] = e.H(0xc388, u);
            q[12] = e.H(0xc3fa, u);
            q[4] = (uint32_t)e.R16s(row + 0x1e);
            e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        }
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
