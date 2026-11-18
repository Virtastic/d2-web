// 1.13c D2Client+0xA26B0: EAX = unit, ESI = esi (a known x), six stack arguments a0..a5, ret 0x18. Builds the 0x21 request block
// (kind a0, unit position + a3/a4, a1/a2 at +0x2C/+0x30) with the target pair (esi, a5) or, when either is zero, the 0xA1340 pair;
// runs 0xB30F0 when both target words are nonzero. Returns its result, else 0.
#include "cl_env.h"
extern "C" uint32_t d2cl_a26b0(uint32_t unit, uint32_t esi, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4,
                               uint32_t a5, const cl::Env& e) {
    uint32_t m[23] = {};
    m[0] = 0x21; m[1] = unit; m[4] = a0;
    m[5] = e.PD(unit, 0xc3be, 0xc) + a3;
    m[6] = e.PD(unit, 0xc2ec, 0x10) + a4;
    m[0xb] = a1; m[0xc] = a2;
    if (esi && a5) { m[7] = esi; m[8] = a5; }
    else {
        e.Ed((uint32_t)(uintptr_t)&m[7]);
        if (!e.H(0xa1340, (uint32_t)(uintptr_t)&m[8], 0, 0, 0, 0, 0, 0, 0, unit)) return 0;
    }
    if (m[7] && m[8]) return e.HB(0xb30f0, (uint32_t)(uintptr_t)m);
    return 0;
}
