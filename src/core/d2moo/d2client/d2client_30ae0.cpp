// 1.13c D2Client+0x30AE0 -- skill description value callback (ECX = unit, EDX = skill, five stack words; word 0 is
// the skills row). The row's base value (+0x1A8) shifted by its shift byte (+0x1A4) less 8, plus the unit's stat
// 0x89, goes to the number formatter (EAX = ECX = the value) with (0, word 2, word 3, word 4).
#include "cl_pa4.h"
extern "C" uint32_t d2c_30ae0(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s2, uint32_t s3, uint32_t s4, const cl::Env& e) {
    if (!edx) return eax;
    if (!s0) return 0;
    const uint32_t sh = (e.R8(s0 + 0x1a4) - 8) & 31;
    uint32_t v = e.R(s0 + 0x1a8) << sh;
    v += e.H(0xc454, ecx, 0x89, 0);
    return e.H(0x2f0a0, 0, s2, s3, s4, 0, v, v);
}
