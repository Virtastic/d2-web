// 1.13c D2Client+0xA22B0: EAX = unit, one stack argument p (a request: +0 / +4 / +8 / +0xC / +0x10), ret 4.
// Runs the unit's 0xC6E8 hook (0xA0660 on p), places the unit at (p+8, p+0xC) through 0xB3EE and 0xB72A, tags a type-1 unit
// whose 0x1A8-byte row signature is 'PB  ' (0x20204250) with the 0xB6F4 byte + 0x38 (mod 0x40), clears with 0xC62E, then runs 0xA1CF0.
#include "cl_env.h"
extern "C" void d2cl_a22b0(uint32_t a, uint32_t p, const cl::Env& e) {
    const uint32_t esi = e.R(p + 8), edi = e.R(p + 0xc);
    if (e.H(0xc6e8, a)) e.H(0xa0660, p);
    e.H(0xc3ee, e.R(a + 0x2c), esi, edi);
    e.H(0xc72a, a, esi, edi);
    if (e.R(a) == 1 && (int32_t)e.R(a + 4) >= 0 && (int32_t)e.R(a + 4) < (int32_t)e.R(e.dt + 0xa80)) {
        const uint32_t rec = e.R(e.dt + 0xa78) + e.R(a + 4) * 0x1a8;
        if (rec && e.R(rec + 0x10) == 0x20204250) {
            const uint32_t c = e.H(0xc6f4, e.R(a + 0x2c)) & 0xff;
            const uint32_t v = (c + 0x38) & 0x3f;
            e.H(0xc2b6, e.R(a + 0x2c), (p & ~0xffu) | v);
        }
    }
    e.H(0xc62e, a, 0);
    e.H(0xa1cf0, e.R(p), e.R(p + 4), e.R(p + 0x10), 0, 0, a);
}
