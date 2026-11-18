// 1.13c D2Client+0xB2720 (ecx = unit, void): without a unit row (or with row short +0x1e negative) run B1720; else
// when ordinal 10985 (the unit's counter) is a multiple of row[0x58] (min 1) call B0A20 with the row short, then tail B17C0.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB2720
extern "C" void d2c_b2720(uint32_t u, const cl::Env& e) {
    uint32_t row = u ? e.UnitRow(u) : 0;
    if (!row || e.R16s(row + 0x1e) < 0) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    int32_t n = (int32_t)e.H(0xc4d2, u);
    int32_t d = (int32_t)e.R(row + 0x58);
    if (d < 1) d = 1;
    if (n % d == 0) e.H(0xb0a20, (uint32_t)e.R16s(row + 0x1e), 0, 0, 0, 0, u);
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
