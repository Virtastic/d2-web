// 1.13c D2Client+0xB0800 (ecx = unit, edx = skip): returns 1 straight away when edx is 0; otherwise, for a unit whose class row has a non-negative
// short at +0x2c, spawns that unit class through D2Client+0xB0450 (the same call as 0xB1080) and returns 0; every other case returns 1.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB0800
extern "C" uint32_t d2c_b0800(uint32_t u, uint32_t skip, const cl::Env& e) {
    if (!skip) return 1;
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x2c) < 0) return 1;
    uint32_t k = e.H(0xc3fa, u);
    uint32_t g = e.H(0xc388, u);
    e.H(0xb0450, (uint32_t)e.R16s(row + 0x2c), 0, 0, g, k, 0, 0, 0, 0, u);
    return 0;
}
