// 1.13c D2Client+0xb5850 -- client missile: spawn the class row's child (word +0x2c) at the unit and hand it the unit's path byte; ECX = unit; returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_b5850(uint32_t u, const cl::Env& e) {
    if (!u) return 1;
    uint32_t row = e.UnitRow(u);
    if (!row) return 1;
    int32_t w = e.R16s(row + 0x2c);
    if (w < 0) return 1;
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!room) return 1;
    uint32_t q[23] = {};
    q[4] = (uint32_t)w;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[1] = room; q[2] = u;
    uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (m) {
        uint32_t x = e.H(0xc2d4, e.R(u + 0x2c));
        e.H(0xc3c4, e.R(m + 0x2c), x);
    }
    return 1;
}
