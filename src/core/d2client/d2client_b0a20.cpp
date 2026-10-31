// 1.13c D2Client+0xB0A20 (eax = unit, stack: other, ret 4): B0450 spawn for `other`, then stat 0x2d4-style hand-off
// of the new unit (ordinal 10706) from the unit's own data (ordinal 10042).
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB0A20
extern "C" void d2c_b0a20(uint32_t u, uint32_t other, const cl::Env& e) {
    uint32_t k = e.H(0xc3fa, u);
    uint32_t g = e.H(0xc388, u);
    uint32_t nu = e.H(0xb0450, other, 0, 0, g, k, 0, 0, 0, 0, u);
    if (!nu) return;
    uint32_t a = e.H(0xc2d4, e.R(u + 0x2c));
    e.H(0xc3c4, e.R(nu + 0x2c), a);
}
