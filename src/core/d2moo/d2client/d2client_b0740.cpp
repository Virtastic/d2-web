// 1.13c D2Client+0xB0740 (ecx = unit, edx = skip): returns 0 when edx is non-zero. Otherwise, for a unit whose class row has a non-negative short at
// +0x2c, spawns that class through D2Client+0xB0450 and, when a unit came back, copies the parent path's 0xC2D4 value onto it (0xC3C4), clears its
// 0xC3E8 field and gives it the 0xC322 value of the parent through 0xC448. Returns 1.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB0740
extern "C" uint32_t d2c_b0740(uint32_t u, uint32_t skip, const cl::Env& e) {
    if (skip) return 0;
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x2c) < 0) return 1;
    uint32_t k = e.H(0xc3fa, u);
    uint32_t g = e.H(0xc388, u);
    uint32_t nu = e.H(0xb0450, (uint32_t)e.R16s(row + 0x2c), 0, 0, g, k, 0, 0, 0, 0, u);
    if (!nu) return 1;
    uint32_t q = e.R(nu + 0x2c);
    uint32_t a = e.H(0xc2d4, e.R(u + 0x2c));
    e.H(0xc3c4, q, a);
    e.H(0xc3e8, q, 0);
    uint32_t t = e.H(0xc322, u);
    e.H(0xc448, nu, t);
    return 1;
}
