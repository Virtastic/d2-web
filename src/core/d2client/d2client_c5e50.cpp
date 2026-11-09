// 1.13c D2Client+0xC5E50, written from the disassembly. ESI = unit, stack (edx0, flag), ret 8. With a path block ([unit+0x2C] nonzero): the
// D2Common 10042 byte p (movzx) goes to 10706(path, (p + (flag ? 0x15 : 0x2A)) % 64) (C remainder, 64 = 0x3F mask with the sign fixup); with
// the d2gfx 10013 mode set the screen offset is D2gfx 10002(11080(unit), 11168(unit), 0x60, &A, &B, 0) shifted by [0x119960] - [0x11C418] in x
// and [0x11995C] - 8 in y, else the unit's 10651 / 11142 coordinates; the 0x6CC00(ECX = unit, EDX = edx0; A, Y, [unit type] == 0, 0) draw
// follows and 10706(path, p) restores the entry. Returns nothing (retmask 0).
#include "cl_mb2.h"
// 1.13c D2Client+0xC5E50
extern "C" uint32_t d2c_c5e50(uint32_t u, uint32_t edx0, uint32_t flag, const clm::Env& e) {
    const uint32_t path = e.R(u + 0x2c);
    if (!path) return 0;
    const uint32_t p = e.H(0xc2d4, 0, 0, 0, 0, 0, path) & 0xff;
    int32_t v = (int32_t)(p + (flag ? 0x15u : 0x2au));
    v = v % 64;
    e.H(0xc3c4, 0, 0, 0, 0, 0, path, (uint32_t)v);
    uint32_t A, Y;
    if (e.H(0xd07e)) {
        uint32_t ab[2] = { 0, 0 };
        const uint32_t r232 = e.H(0xc232, 0, 0, 0, 0, 0, u);
        const uint32_t r1ae = e.H(0xc1ae, 0, 0, 0, 0, 0, u);
        const auto P = [](const void* q) { return (uint32_t)(uintptr_t)q; };
        e.H(0xd132, 0, 0, 0, 0, 0, r1ae, r232, 0x60, P(&ab[0]), P(&ab[1]), 0);
        A = ab[0] + (e.G(0x119960) - e.G(0x11c418));
        Y = ab[1] + e.G(0x11995c) - 8;
    } else {
        A = e.H(0xc1d2, 0, 0, 0, 0, 0, u);
        Y = e.H(0xc1f6, 0, 0, 0, 0, 0, u);
    }
    e.H(0x6cc00, 0, u, edx0, 0, 0, A, Y, e.R(u) == 0, 0);
    e.H(0xc3c4, 0, 0, 0, 0, 0, path, p);
    return 0;
}
