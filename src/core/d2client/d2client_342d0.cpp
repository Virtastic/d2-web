// 1.13c D2Client+0x342D0 -- skill description line callback (ECX = unit, EDX = skill, five stack words; word 0 is
// the skills row, word 1 the level). Evaluates the row's calc at +0x138 (unit, calc, skill id, level) and hands the
// result to the shared line builder with the fixed formatter, EAX = the row, ECX = the skill.
#include "cl_pa4.h"
extern "C" uint32_t d2c_342d0(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, const cl::Env& e) {
    const uint32_t r = e.H(0xc316, ecx, e.R(s0 + 0x138), (uint32_t)e.R16s(s0), s1);
    const uint32_t a = e.H(0x30b40, ecx, s1, 0x6FAE2F30u, r, 0, s0, edx);
    pa4::xw(e, 0x30b40, {0, s2, s3, s4, 1});
    return a;
}
