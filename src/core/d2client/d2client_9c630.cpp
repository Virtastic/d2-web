// 1.13c D2Client+0x9C630 -- left-button-up handler of the 0xFAD84 panel (stdcall, one stack word = the message record P, ret 4, void).
// x = u16[P+0xC], y = u16[P+0xE]; the player's word at +0x60 is read first (stack word of 0xC202). 0x11BEDC / 0x11BEE0 := 0. 0x26320(EAX = P)
// answering nonzero skips to the tail. Otherwise 0x8BD90 (ECX x, EDX y): zero runs 0xC202(player+0x60) and the tail; nonzero closes the panel:
// old = 0xFAD84, 0xBF8C, the count at 0xDD5D0 must be 0x26 (else the 0x139B fatal path), 0xFAD84 := 0; old nonzero runs 0xBF720, 0xD0B4(0xD05B0),
// 0x3CC60 (EAX 4; w, 0xD05B0) and 0xBE2E0; then 0x8BBF0. With 0xFAD88, 0xFADD0, 0xFADBC, 0xFADC0, 0xFAE10, 0xFAE14 and 0xFADD8 all clear:
// 0x11C418 / 0x11C414 := 0, 0xD1B0 (ECX 0), 0x8A4D0 (EAX 0xF9E18, ECX 0xF9E14, EDX 0, ESI 0x11C3C4, stack 0), return. Else 0x11C418 :=
// width/4 (0xDBC48), 0x11C414 := 2, 0xD1B0 (ECX = it), 0x8A4D0 (ECX 0xF9E14 plus a quarter of it, stack = the quarter), return.
// The tail: with 0x11BE7C set and 0x11BC34 clear, 0x11BE7C := 0 and 0x8D3D0(EAX y): nonzero with 0x11BE78 clear runs 0xC30E0(1). The four button
// latches clear; when x is in [0x10B2F8, 0x10B2FC) and y in [0x10B300, 0x10B304] and 0x26320(EAX = P) answers 0: P[+0x1C] = 0, P[+0x18] = 1, 0xBE84(P).
#include "cl_pa4.h"
static int32_t q4_9c630(int32_t x) { return (x + ((x >> 31) & 3)) >> 2; }
extern "C" uint32_t d2c_9c630(uint32_t P, const cl::Env& e) {
    using namespace pa4;
    const uint32_t pl60 = e.R(G(e, 0x11bbfc) + 0x60);
    const int32_t x = (int32_t)P16(e, P + 0xc), y = (int32_t)P16(e, P + 0xe);
    S(e, 0x11bedc, 0);
    S(e, 0x11bee0, 0);
    // EAX at the final ret 4 is the last value left in it: the 0x26320 answer, then the 0xC202 / 0x8D3D0 / 0xC30E0 / second 0x26320 / 0xBE84 results
    uint32_t eax = e.H(0x26320, 0, 0, 0, 0, 0, P);
    if (eax == 0) {
        const uint32_t hit = e.H(0x8bd90, 0, 0, 0, 0, 0, 0, (uint32_t)x, (uint32_t)y);
        eax = hit;
        if (hit != 0) {
            const uint32_t old = G(e, 0xfad84);
            e.H(0xbf8c);
            if (G(e, 0xdd5d0) != 0x26) assertAt(e, 0x139b);
            S(e, 0xfad84, 0);
            if (old) {
                e.H(0xbf720);
                const uint32_t t = e.H(0xd0b4, 0x6FB805B0u);
                e.H(0x3cc60, t, 0x6FB805B0u, 0, 0, 0, 4);
                e.H(0xbe2e0);
            }
            e.H(0x8bbf0);
            if (!G(e, 0xfad88) && !G(e, 0xfadd0) && !G(e, 0xfadbc) && !G(e, 0xfadc0) && !G(e, 0xfae10) && !G(e, 0xfae14) && !G(e, 0xfadd8)) {
                S(e, 0x11c418, 0);
                S(e, 0x11c414, 0);
                e.H(0xd1b0, 0, 0, 0, 0, 0, 0, 0);
                return e.H(0x8a4d0, 0, 0, 0, 0, 0, G(e, 0xf9e18), G(e, 0xf9e14), 0, 0, G(e, 0x11c3c4));
            } else {
                const int32_t q = q4_9c630((int32_t)G(e, 0xdbc48));
                S(e, 0x11c418, (uint32_t)q);
                S(e, 0x11c414, 2);
                e.H(0xd1b0, 0, 0, 0, 0, 0, 0, (uint32_t)q);
                const int32_t c = (int32_t)G(e, 0xf9e14), t = q4_9c630(c);
                return e.H(0x8a4d0, (uint32_t)t, 0, 0, 0, 0, G(e, 0xf9e18), (uint32_t)(c + t), 0, 0, G(e, 0x11c3c4));
            }
        }
        eax = e.H(0xc202, pl60);
    }
    if (G(e, 0x11be7c) != 0 && G(e, 0x11bc34) == 0) {
        S(e, 0x11be7c, 0);
        eax = e.H(0x8d3d0, 0, 0, 0, 0, 0, (uint32_t)y);
        if (eax != 0 && G(e, 0x11be78) == 0) eax = e.H(0xc30e0, 1);
    }
    clearButtons(e);
    if (x >= (int32_t)G(e, 0x10b2f8) && x < (int32_t)G(e, 0x10b2fc) && y >= (int32_t)G(e, 0x10b300) && y <= (int32_t)G(e, 0x10b304)) {
        eax = e.H(0x26320, 0, 0, 0, 0, 0, P);
        if (eax == 0) eax = fin(e, P);
    }
    return eax;
}
