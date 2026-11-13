// 1.13c D2Client+0xB0EE0 (ecx = unit, edx = skip): for a unit whose class row has a non-negative short at +0x2c, returns 0 when edx is non-zero and
// otherwise calls D2Client+0xB0A20 with that short. Every other case returns 1.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB0EE0
extern "C" uint32_t d2c_b0ee0(uint32_t u, uint32_t skip, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x2c) < 0) return 1;
    if (skip) return 0;
    e.H(0xb0a20, (uint32_t)e.R16s(row + 0x2c));
    return 1;
}
