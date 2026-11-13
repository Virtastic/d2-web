// 1.13c D2Client+0xB9BB0 (ecx = unit): when the unit's missile row has a +0x2C child class, run the 0xB6BC0 ring with that class and max(row +0x6C, 1) / 2. Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_b9bb0(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x2c) < 0) return 1;
    int32_t n = (int32_t)e.R(row + 0x6c);
    if (n < 1) n = 1;
    e.H(0xb6bc0, (uint32_t)n, 2, 0, 0, 0, u, 0, 0, (uint32_t)e.R16s(row + 0x2c));
    return 1;
}
