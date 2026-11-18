// 1.13c D2Client+0x35470 -- the sibling of 0x353A0 for the other pair of description lines (SkillDesc.cpp:0x9B8 and 0x9B9) and the
// final flag 1 on the shared writer 0x30B40; same registers and stack words, same early exits (null skill, row or skill record,
// or the plain builder 0x34C00 when #10747 on the unit fails).
#include "cl_pa4.h"
extern "C" uint32_t d2c_35470(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, const cl::Env& e) {
    if (!edx || !s0) return eax;
    const uint32_t rec = e.H(0x17f0, 0, 0, 0, 0, 0, (uint32_t)e.R16s(s0));
    if (!rec) return 0;
    if (!e.H(0xc6e8, ecx)) {
        const uint32_t r = e.H(0x34c00, s0, s1, s2, s3, s4, 0, ecx, edx);
        pa4::xw(e, 0x34c00, {s4});
        return r;
    }
    const uint32_t a = e.H(0xc42a, edx, 0x6FB86990u, 0x9b8, s1);
    const uint32_t v1 = e.H(0xcee0, ecx, e.R(rec + 0x18), a);
    const uint32_t b = e.H(0xc42a, edx, 0x6FB86990u, 0x9b9, s1);
    const uint32_t v2 = e.H(0xcee0, ecx, e.R(rec + 0x1c), b);
    const uint32_t r = e.H(0x30b40, ecx, s1, 0x6FAE3240u, v1, 0, s0, edx);
    pa4::xw(e, 0x30b40, {v2, s2, s3, s4, 1});
    return r;
}
