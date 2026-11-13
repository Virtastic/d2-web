// 1.13c D2Client+0x57800 -- skill do function (ECX = unit, EDX = skill id, one stack word): when the skill row's
// missile word (+0xEA) names a missiles row the shared launcher runs with (missile, skill id, the stack word),
// EAX = the unit, and 1 is returned; otherwise 0.
#include "cl_pa4.h"
extern "C" uint32_t d2c_57800(uint32_t ecx, uint32_t edx, uint32_t s0, const cl::Env& e) {
    const uint32_t row = pa4::skRow(e, edx);
    const int32_t w = e.R16s(row + 0xea);
    if (w < 0 || w >= (int32_t)e.R(e.dt + 0xb6c)) return 0;
    e.H(0xa15c0, (uint32_t)w, edx, s0, 0, 0, ecx);
    return 1;
}
