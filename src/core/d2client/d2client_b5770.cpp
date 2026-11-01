// 1.13c D2Client+0xb5770 -- client missile: spawn the two class-row children (words +0x2c, +0x2e) at the unit; the second one's motion block is cleared and stamped (row+0x6c << 11); ECX = unit; returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_b5770(uint32_t u, const cl::Env& e) {
    if (!u) return 1;
    uint32_t row = e.UnitRow(u);
    if (!row) return 1;
    uint32_t q[23] = {};
    q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    q[2] = u;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    int32_t w = e.R16s(row + 0x2c);
    if (w >= 0) { q[4] = (uint32_t)w; e.HB(0xb30f0, (uint32_t)(uintptr_t)q); }
    w = e.R16s(row + 0x2e);
    if (w < 0) return 1;
    q[4] = (uint32_t)w;
    uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (!m) return 1;
    uint32_t pa = e.R(m + 0x54);
    uint32_t sh = e.R(row + 0x6c);
    if (!pa) return 1;
    uint32_t mo = e.R(pa + 0x30);
    if (!mo) return 1;
    e.W(mo + 4, 0); e.W(mo + 8, 0); e.W(mo + 0xc, sh << 11);
    return 1;
}
