// 1.13c D2Client+0xC9A50: EAX unit, seven stack arguments a0..a6 (ret 0x1C). Disassembly-derived.
// 0xA1340(ebx unit, EDI -> a3's own slot = X, word 0 -> Y) must succeed (else 0). With a5: a0 -= 1 and r = 0xA15C0(eax unit;
// a1, a2, a3); on r: a6 ? 0xC3E8(r[0x2C], 0) and 0xC2FE(r, a4). Block {a6 ? 0x2021 : 0x21, unit, 0,0, a1, ux, uy, X, Y, ...,
// [11] a2, [12] a3, [21] callback 0x6FB23B60, [22] i} through 0xB30F0 for i in [0, a0), each r handled as above. Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2cl_c9a50(uint32_t unit, int32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6,
                               const cl::Env& e) {
    int32_t X = (int32_t)a3, Y = 0;
    e.Ed((uint32_t)(uintptr_t)&X);
    if (!e.H(0xa1340, (uint32_t)(uintptr_t)&Y, 0, 0, 0, 0, 0, 0, 0, unit)) return 0;
    if (a5) {
        --a0;
        const uint32_t r = e.H(0xa15c0, a1, a2, a3, 0, 0, unit);
        if (r) {
            if (a6) e.H(0xc3e8, e.R(r + 0x2c), 0);
            e.H(0xc2fe, r, a4);
        }
    }
    uint32_t d[23] = {0};
    d[0] = a6 ? 0x2021 : 0x21; d[1] = unit; d[4] = a1;
    d[5] = e.PD(unit, 0xc3be, 0xc); d[6] = e.PD(unit, 0xc2ec, 0x10);
    d[7] = (uint32_t)X; d[8] = (uint32_t)Y; d[11] = a2; d[12] = a3;
    d[21] = 0xB2073B60u;   // module pointer 0x6FB23B60, recorded module-relative
    for (int32_t i = 0; i < a0; ++i) {
        d[22] = (uint32_t)i;
        const uint32_t r = e.HB(0xb30f0, (uint32_t)(uintptr_t)d);
        if (!r) continue;
        if (a6) e.H(0xc3e8, e.R(r + 0x2c), 0);
        e.H(0xc2fe, r, a4);
    }
    return 1;
}
