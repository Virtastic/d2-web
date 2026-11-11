// 1.13c D2Client+0xA2F60: ECX = unit, EDX = ebp (a request argument), one unused stack argument, ret 4.
// Marks the unit dirty, resolves 0xC6EE(unit, 0); when the result's 0xC220 type is 0x26 the 0xB9A80 request is built from the
// 0xA1340 position pair, otherwise 0xA15C0 runs. Returns 0 when 0xC6EE finds nothing, else 1.
#include "cl_env.h"
#include "cl_a2.h"
extern "C" uint32_t d2cl_a2f60(uint32_t unit, uint32_t ebp, const cl::Env& e) {
    if (unit) e.W(unit + 0xc4, e.R(unit + 0xc4) | 0x40);
    const uint32_t edi = e.H(0xc6ee, unit, 0);
    if (!edi) return 0;
    const uint32_t esi = e.H(0xc736, edi);
    const uint32_t t = e.H(0xc220, edi);
    if (t == 0x26) {
        uint32_t x = 0, y = 0;
        e.Ed((uint32_t)(uintptr_t)&x);
        e.H(0xa1340, (uint32_t)(uintptr_t)&y, 0, 0, 0, 0, 0, 0, 0, unit);
        const uint32_t w[8] = { esi, 0, 0, x, y, ebp, 1, 0 };
        cl::b9a80(e, unit, w);
        return 1;
    }
    e.H(0xa15c0, esi, ebp, 1, 0, 0, unit);
    return 1;
}
