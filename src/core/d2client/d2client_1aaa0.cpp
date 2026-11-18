// 1.13c D2Client+0x1AAA0: EDI = struct (+4 unit, +0xC, +0x10), EAX passes through on the early exits; type-0 unit with path: modes 2/6 -> 0x1A010(1), mode 3 -> 0x1A010(3)
#include "cl17.h"
extern "C" uint32_t d2c_1aaa0(uint32_t eax, uint32_t edi, const cl::Env& e) {
    uint32_t esi = e.R(edi + 4);
    if (!esi || e.R(esi) != 0) return eax;
    e.H(0x2a0b0, 0, 0, 0, 0, 0, esi);
    eax = e.R(esi + 0x2c);                 // 0 -> the error-log + exit path, never vectored
    eax = e.H(0xc2da, eax);
    if (eax) return eax;
    uint32_t m = e.R(esi + 0x10) - 2;
    uint32_t act;
    if (m == 0) act = 1;
    else if (m == 1) act = 3;
    else { m -= 4; if (m) return m; act = 1; }
    return e.H(0x1a010, act, esi, 0, 0, 0, e.R(edi + 0x10), e.R(edi + 0xc));
}
