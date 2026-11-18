// 1.13c D2Client+0xb5a10 -- client missile: type-0x21 child at the unit with the class row's word as id; stamps its motion block (flag 4, row +0x6c/+0x70), runs the 0xC7C80 setup with row +0x74, clears flag 2; ECX = unit; returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_b5a10(uint32_t u, const cl::Env& e) {
    if (!u) return 1;
    uint32_t row = e.UnitRow(u);
    if (!row) return 1;
    int32_t w = e.R16s(row + 0x2c);
    if (w < 0) return 1;
    uint32_t q[23] = {};
    q[0] = 0x21;
    q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    q[5] = e.PD(u, 0xc3be, 0xc);
    q[6] = e.PD(u, 0xc2ec, 0x10);
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[4] = (uint32_t)w;
    q[7] = e.H(0xc31c, e.R(u + 0x2c));
    q[8] = e.H(0xc340, e.R(u + 0x2c));
    uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (!m) return 1;
    uint32_t pa = e.R(m + 0x54);
    uint32_t a = e.R(row + 0x70), b = e.R(row + 0x6c);
    if (pa) {
        uint32_t mo = e.R(pa + 0x30);
        if (mo) { e.W(mo, e.R(mo) | 4); e.W(mo + 0x40, b); e.W(mo + 0x44, a); }
    }
    e.H(0xc7c80, 0, 0, 0, 0, 0, m, e.R(row + 0x74));
    pa = e.R(m + 0x54);
    if (pa) {
        uint32_t mo = e.R(pa + 0x30);
        if (mo) e.W(mo, e.R(mo) & ~2u);
    }
    return 1;
}
