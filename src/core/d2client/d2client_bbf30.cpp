// 1.13c D2Client+0xbbf30 -- missile callback: one child at the unit. ECX = unit, EDX = a2; returns 0 when a2 is set on a
// valid Missiles row whose s16 +0x2c (child class) is >= 0, else 1. The child: m = 11029(u), r = 10240(u), then 0xBB210
// (EAX = u) with (row+0x6c, child, r, m, row+0x70, 1).
#include <cstdint>
#include "cl_env.h"
extern "C" uint32_t d2c_bbf30(uint32_t u, uint32_t a2, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x2c) < 0) return 1;
    if (a2) return 0;
    uint32_t w70 = e.R(row + 0x70);
    uint32_t m = e.H(0xc3fa, u);
    uint32_t r = e.H(0xc388, u);
    uint32_t w[6] = { e.R(row + 0x6c), (uint32_t)e.R16s(row + 0x2c), r, m, w70, 1 };
    e.H(0xbb210, w[0], w[1], w[2], w[3], w[4], u);
    if (e.blk) e.blk(e.ctx, 0xbb210, (uint32_t)(uintptr_t)w, 6);
    return 1;
}
