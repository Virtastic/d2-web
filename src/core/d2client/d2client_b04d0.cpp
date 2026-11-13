// 1.13c D2Client+0xB04D0 (ecx = unit): one LCG step on the unit seed (+0x20); on an even result a second step picks the class (unit class + 1 +
// the second result's low bit) spawned through D2Client+0xB0450, and the spawned unit's path takes the parent's 0xC2D4 value (0xC3C4) and a cleared
// 0xC3E8 field. Always returns 1.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB04D0
extern "C" uint32_t d2c_b04d0(uint32_t u, const cl::Env& e) {
    if (e.Lcg(u + 0x20) & 1) return 1;
    uint32_t cls = e.R(u + 4);
    uint32_t bit = e.Lcg(u + 0x20) & 1;
    uint32_t k = e.H(0xc3fa, u);
    uint32_t g = e.H(0xc388, u);
    uint32_t nu = e.H(0xb0450, bit + cls + 1, 0, 0, g, k, 0, 0, 0, 0, u);
    if (!nu) return 1;
    uint32_t p = e.R(u + 0x2c), q = e.R(nu + 0x2c);
    uint32_t a = e.H(0xc2d4, p);
    e.H(0xc3c4, q, a);
    e.H(0xc3e8, q, 0);
    return 1;
}
