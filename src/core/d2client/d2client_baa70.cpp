// 1.13c D2Client+0xBAA70 (ecx = unit): run the 0xB5B70 spiral with the unit's missile row (+0x6C count; +0x70, +0x2C, +0x2E as the three stack words). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_baa70(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row) return 1;
    e.H(0xb5b70, e.R(row + 0x70), (uint32_t)e.R16s(row + 0x2c), (uint32_t)e.R16s(row + 0x2e), 0, 0, e.R(row + 0x6c), u);
    return 1;
}
