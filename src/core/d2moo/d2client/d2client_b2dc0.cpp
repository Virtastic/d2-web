// 1.13c D2Client+0xB2DC0 (ecx = unit, void): when ordinal 11372 is non-zero set bit 16 of the unit's flags (+0xc4),
// release the pointer at +0x64 through A8630 (esi) and clear it; otherwise tail B17C0.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB2DC0
extern "C" void d2c_b2dc0(uint32_t u, const cl::Env& e) {
    if (!e.H(0xc46c, u)) { e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u); return; }
    if (u) e.W(u + 0xc4, e.R(u + 0xc4) | 0x10000);
    uint32_t s = e.R(u + 0x64);
    if (s) { e.H(0xa8630, 0, 0, 0, 0, 0, 0, 0, 0, 0, s); e.W(u + 0x64, 0); }
}
