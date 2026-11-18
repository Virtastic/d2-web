// 1.13c D2Client+0x6D280: window-rect clip test for a sprite handle. ESI = handle, stack a0 a1 a2 (ret 0xC). Each query is a one-word import:
// 0xC0E2 (retail calls it with no pushed word although it ends `ret 4`: it eats the saved ESI, so its record carries only s0), 0xC0DC, 0xC0CA, 0xC0D6.
// ebx = C0DC + a1, edi = C0CA + a0, eax = C0D6; a2 != 0: edi -= c0e2, eax += c0e2. e.gl = the two screen limits at +0xDBC48 / +0xDBC4C (signed).
// Returns 0 when edi > limit0, eax + edi < 0 or ebx - c0e2 > limit1, else (ebx + c0e2 >= 0).
#include "cl_env.h"
extern "C" uint32_t d2cl_6d280(uint32_t h, uint32_t a0, uint32_t a1, uint32_t a2, const cl::Env& e) {   // 1.13c D2Client+0x6D280
    const uint32_t ebp = e.H(0xc0e2, h);
    uint32_t ebx = e.H(0xc0dc, h) + a1;
    uint32_t edi = e.H(0xc0ca, h) + a0;
    uint32_t eax = e.H(0xc0d6, h);
    if (a2) { edi -= ebp; eax += ebp; }
    if ((int32_t)edi > (int32_t)e.R(e.gl)) return 0;
    if ((int32_t)(eax + edi) < 0) return 0;
    if ((int32_t)(ebx - ebp) > (int32_t)e.R(e.gl + 4)) return 0;
    return (int32_t)(ebx + ebp) >= 0;
}
