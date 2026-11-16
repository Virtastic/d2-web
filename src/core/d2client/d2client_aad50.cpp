// 1.13c D2Client+0xAAD50 -- keyboard message handler of the panel that toggles at 0xFADD0 (stdcall, one stack word = the message record P, ret 4,
// void). P[+4] = 0x102 with P[+8] = 9, or 0x104 with 0x73, is ignored. Otherwise: 0xAA380, the old value of 0xFADD0, 0xBF8C, the table count at
// 0xDD5D0 must be 0x26 (else the 0x139B fatal path), 0xFADD0 := 0; an old nonzero value runs 0xD0B4(0xD0828), 0x3CC60 (EAX 5; w, 0xD0828),
// 0xAA380 and 0xBE2E0. Then the view shift: with 0xFAD84 and 0xFAD90 clear, 0x11C418 / 0x11C414 := 0 and 0xD1B0 (ECX 0), 0x8A4D0 (EAX 0xF9E18,
// ECX 0xF9E14, EDX 0, ESI 0x11C3C4, stack 0) and, when the word at 0x11B828 (EBX) is past half the screen width at 0xDBC48, 0x161B0 (EBX = it
// less a quarter of the width, EDI = 0x11B824); otherwise 0x11C418 := -(width/4), 0x11C414 := 1, 0xD1B0 (ECX = 0x11C418) and 0x8A4D0 (ECX
// 0xF9E14 less a quarter of it, stack = minus that quarter). The handled tail: P[+0x18] = 1, P[+0x1C] = 0, 0xBE84(P).
#include "cl_pa4.h"
static int32_t q4_aad50(int32_t x) { return (x + ((x >> 31) & 3)) >> 2; }
extern "C" uint32_t d2c_aad50(uint32_t P, const cl::Env& e) {
    using namespace pa4;
    const uint32_t m = e.R(P + 4), w = e.R(P + 8);
    if ((m == 0x102 && w == 9) || (m == 0x104 && w == 0x73)) return m;   // EAX still holds the message code at the ignored-key ret
    e.H(0xaa380);
    const uint32_t old = G(e, 0xfadd0);
    e.H(0xbf8c);
    if (G(e, 0xdd5d0) != 0x26) assertAt(e, 0x139b);
    S(e, 0xfadd0, 0);
    if (old) {
        const uint32_t t = e.H(0xd0b4, 0x6FB80828u);
        e.H(0x3cc60, t, 0x6FB80828u, 0, 0, 0, 5);
        e.H(0xaa380);
        e.H(0xbe2e0);
    }
    const uint32_t ebx = G(e, 0x11b828), edi = G(e, 0x11b824);
    if (G(e, 0xfad84) == 0 && G(e, 0xfad90) == 0) {
        S(e, 0x11c418, 0);
        S(e, 0x11c414, 0);
        e.H(0xd1b0, 0, 0, 0, 0, 0, 0, 0);
        e.H(0x8a4d0, 0, 0, 0, 0, 0, G(e, 0xf9e18), G(e, 0xf9e14), 0, ebx, G(e, 0x11c3c4));
        const int32_t W = (int32_t)G(e, 0xdbc48);
        if ((int32_t)ebx > ((W - (W >> 31)) >> 1)) {
            const int32_t q = q4_aad50(W);
            e.H(0x161b0, 0, 0, 0, 0, 0, 0, 0, 0, ebx - (uint32_t)q);
            e.Edj(0x161b0, edi);
        }
    } else {
        const int32_t q = q4_aad50((int32_t)G(e, 0xdbc48));
        S(e, 0x11c418, (uint32_t)-q);
        S(e, 0x11c414, 1);
        e.H(0xd1b0, 0, 0, 0, 0, 0, 0, (uint32_t)-q);
        const int32_t c = (int32_t)G(e, 0xf9e14), t = q4_aad50(c);
        e.H(0x8a4d0, (uint32_t)-t, 0, 0, 0, 0, G(e, 0xf9e18), (uint32_t)(c - t), 0, ebx, G(e, 0x11c3c4));
    }
    return fin(e, P);
}
