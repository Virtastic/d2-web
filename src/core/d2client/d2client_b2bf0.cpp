// 1.13c D2Client+0xB2BF0 (ecx = unit, void): for each of the three row shorts (+0x1e/+0x20/+0x22) that are positive and
// whose trigger count (row +0x58/+0x5c/+0x60) equals the unit's counter, call AFF10 (edx = the A60D0 partner) with the
// short, the unit position and the C388 value. Missing row runs B1720; then tail B17C0.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB2BF0
extern "C" void d2c_b2bf0(uint32_t u, const cl::Env& e) {
    uint32_t row = u ? e.UnitRow(u) : 0;
    if (!row) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    for (int i = 0; i < 3; i++) {
        int32_t sh = e.R16s(row + 0x1e + 2 * i);
        if (sh <= 0) continue;
        if ((int32_t)e.H(0xc4d2, u) != (int32_t)e.R(row + 0x58 + 4 * i)) continue;
        uint32_t a3 = e.H(0xc3fa, u);
        uint32_t r = e.H(0xc388, u, a3);
        uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
        uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
        uint32_t p = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
        e.H(0xaff10, (uint32_t)sh, x, y, r, 0, 0, 0, p);
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
