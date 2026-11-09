// 1.13c D2Client+0xC64B0, written from the disassembly. ECX = unit, one unused stack word (ret 4). path = [unit+0x2C]; p = 0xC838(ECX = unit); no p or no path
// returns 0. a = 10344(p) (0xCB68), b = 10350(p) (0xCB6E), m = 0xC628(p). m&3 == 3: size = type ? 0x100 : 0x80 (type = [unit]); cd = 0xCD30(unit)
// (it takes ONE word, so `size` stays on the stack as the fifth argument of 0xCB98), 0xCB98(0xC1F0(unit), a, b, cd, size), 0xC400(a, size),
// 0xC3AC(a, type ? 0x3C01 : 0x1C09), 0xC60A(p, 0), then 0x88BF0(EDX = 0x225D0(EAX = 0x9E3, ECX = unit)). Else m&1: 0xCB56(unit, 3), [unit+0x48] = 0x500. Returns 1.
#include "cl_mb2.h"
// 1.13c D2Client+0xC64B0
extern "C" uint32_t d2c_c64b0(uint32_t u, uint32_t, const clm::Env& e) {
    const uint32_t path = e.R(u + 0x2c);
    const uint32_t p = e.H(0xc838, 0, u);
    if (!p || !path) return 0;
    const uint32_t type = e.R(u);
    const uint32_t size = type == 0 ? 0x80 : 0x100;
    const uint32_t a = e.H(0xcb68, 0, 0, 0, 0, 0, p);
    const uint32_t b = e.H(0xcb6e, 0, 0, 0, 0, 0, p);
    const uint32_t m = e.H(0xc628, 0, 0, 0, 0, 0, p);
    if ((m & 3) == 3) {
        const uint32_t cd = e.H(0xcd30, 0, 0, 0, 0, 0, u);
        const uint32_t f = e.H(0xc1f0, 0, 0, 0, 0, 0, u);
        e.H(0xcb98, 0, 0, 0, 0, 0, f, a, b, cd, size);
        e.H(0xc400, 0, 0, 0, 0, 0, a, size);
        e.H(0xc3ac, 0, 0, 0, 0, 0, a, type == 0 ? 0x1c09u : 0x3c01u);
        e.H(0xc60a, 0, 0, 0, 0, 0, p, 0);
        const uint32_t k = e.H(0x225d0, 0x9e3, u);
        e.H(0x88bf0, 0, 0, k);
        return 1;
    }
    if (m & 1) {
        e.H(0xcb56, 0, 0, 0, 0, 0, u, 3);
        e.W(u + 0x48, 0x500);
    }
    return 1;
}
