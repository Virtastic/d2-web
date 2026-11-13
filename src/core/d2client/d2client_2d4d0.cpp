// 1.13c D2Client+0x2D4D0 -- skill description line callback (ECX = unit, EDX = skill, six stack words): clears the
// two output slots (words 2 and 3) and forwards to the plain line builder with (slot A, slot B, word 1, EDX),
// EAX = 0, ESI = the unit. Its answer is returned.
#include "cl_pa4.h"
extern "C" uint32_t d2c_2d4d0(uint32_t ecx, uint32_t edx, uint32_t s1, uint32_t s2, uint32_t s3, const cl::Env& e) {
    e.W(s2, 0);
    e.W(s3, 0);
    return e.H(0x2c2d0, s2, s3, s1, edx, 0, 0, 0, 0, 0, ecx);
}
