// 1.13c D2Client+0xB1080 (ecx = unit): spawn through D2Client+0xB0450 a unit whose id is picked from the unit
// row's signed 16-bit range (+0x2c .. +0x2e), rolled on the unit's seed when the range is non-empty. Returns 1.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB1080
extern "C" uint32_t d2c_b1080(uint32_t u, const cl::Env& e) {
    if (!u) return 1;
    uint32_t row = e.UnitRow(u);
    if (!row) return 1;
    int32_t lo = e.R16s(row + 0x2c), hi = e.R16s(row + 0x2e);
    if (lo < 0) return 1;
    int32_t v = lo;
    if (hi > lo) v += (int32_t)e.H(0x1380, 0, 0, 0, 0, 0, 0, u + 0x20, 0, 0, (uint32_t)(hi - lo + 1));
    uint32_t k = e.H(0xc3fa, u);
    uint32_t g = e.H(0xc388, u);
    e.H(0xb0450, (uint32_t)v, 0, 0, g, k, 0, 0, 0, 0, u);
    return 1;
}
