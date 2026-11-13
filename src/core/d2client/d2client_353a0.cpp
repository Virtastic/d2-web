// 1.13c D2Client+0x353A0 -- skill description line from a pair of table strings (ECX = unit, EDX = skill, five stack words;
// word 0 is the skills row, word 1 the text argument). Nothing for a null skill, row or skill record; when #10747 on the unit
// fails the line goes to the plain builder 0x34C00 with the stack words passed through. Otherwise both source lines
// (SkillDesc.cpp:0x9DD and 0x9DE) are resolved through 0xC42A and the record's two string slots (+0x18, +0x1C) through 0xCEE0, and the
// shared description writer 0x30B40 gets them with callback 0x6FAE3240 and the final flag 0 (nine stack words: unit, text, callback,
// first result, second result, the three pass-through words, flag; EAX = the row, ECX = the skill).
#include "cl_pa4.h"
extern "C" uint32_t d2c_353a0(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, const cl::Env& e) {
    if (!edx || !s0) return eax;
    const uint32_t rec = e.H(0x17f0, 0, 0, 0, 0, 0, (uint32_t)e.R16s(s0));
    if (!rec) return 0;
    if (!e.H(0xc6e8, ecx)) {
        const uint32_t r = e.H(0x34c00, s0, s1, s2, s3, s4, 0, ecx, edx);
        pa4::xw(e, 0x34c00, {s4});
        return r;
    }
    const uint32_t a = e.H(0xc42a, edx, 0x6FB86990u, 0x9dd, s1);
    const uint32_t v1 = e.H(0xcee0, ecx, e.R(rec + 0x18), a);
    const uint32_t b = e.H(0xc42a, edx, 0x6FB86990u, 0x9de, s1);
    const uint32_t v2 = e.H(0xcee0, ecx, e.R(rec + 0x1c), b);
    const uint32_t r = e.H(0x30b40, ecx, s1, 0x6FAE3240u, v1, 0, s0, edx);
    pa4::xw(e, 0x30b40, {v2, s2, s3, s4, 0});
    return r;
}
