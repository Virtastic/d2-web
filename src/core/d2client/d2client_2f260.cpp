// 1.13c D2Client+0x2F260 -- skill description line callback (ECX = unit, EDX = skill, six stack words): runs the
// shared two-column builder (EAX = the unit) with the four slots, then moves the second pair of results into the
// first pair and clears the second pair. EAX is the value moved last.
#include "cl_pa4.h"
extern "C" uint32_t d2c_2f260(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, const cl::Env& e) {
    (void)s0;
    e.H(0x2d500, s2, s3, 0, s4, s5, ecx);
    pa4::xw(e, 0x2d500, {s5, 0, edx, 0x6FADD6C0u, 0, s1});
    e.W(s2, e.R(s4));
    const uint32_t v = e.R(s5);
    e.W(s3, v);
    e.W(s4, 0);
    e.W(s5, 0);
    return v;
}
