// 1.13c D2Client+0x7F260 -- left-button-down handler of the bottom-left button that toggles the 0xFADC0 panel (stdcall, one stack word = the message
// record P, ret 4, void; EAX modelled). The click must lie left of half the width (0xDBC48) and above height - 0x30 (0xDBC4C), else the call
// returns (EAX = y, or W/2). 0xBF8C, the count at 0xDD5D0 must be 0x26 (else the 0x139B fatal path, execution continues). old = 0xFADC0. With old
// zero and 0x113A68 clear each panel i below the count whose latch at 0xFAD80[i] is 1 acts on the kind word at (0xDD538[i])+0x40: 0 nothing,
// 1 0xC2790 (ECX i, EDX 1, stack 0), 4 with 0x119721 set 0x47B20 (EDI = the dword at 0x11971D), 2, 3 and past 4 jump to the tail (0x113A68
// set skips the walk the same way). Then cur = 0xFADC0 := (cur == 0) when there is no player (0x11BBFC) or the player's first word is 0 and
// 0xA5440 (EAX = the player) answers 0; else cur is left. Old zero with cur set: 0xD0B4(0xD0810) and 0x3CCC0 (EAX 2); old set with cur clear:
// 0xD0B4, 0x3CC60 (EAX 2) and 0xBE2E0. Then the view shift on 0x11C418 / 0x11C414 (modes 0, 2, 3, 1) with 0xD1B0 (ECX) and 0x8A4D0 (EAX 0xF9E18,
// ECX 0xF9E14 +- a quarter, EDX 0, ESI 0x11C3C4, stack = the quarter or 0), mode 3 clears the four button latches and jumps to the tail. The tail:
// 0x7AD00, P[+0x18] = 1, P[+0x1C] = 0, 0xBE84(P), the latches 0x11C3DC / 3E0 / 3F8 / 3FC, 0x11C2F8 / 2F4 := 0, 0xF46C8 / 0xF46C4 := 0x10, EAX 0x10.
#include "cl_pa4.h"
static int32_t q4_7f260(int32_t x) { return (x + ((x >> 31) & 3)) >> 2; }
static uint32_t tail_7f260(const cl::Env& e, uint32_t P) {
    using namespace pa4;
    e.H(0x7ad00);
    e.W(P + 0x18, 1);
    e.W(P + 0x1c, 0);
    e.H(0xbe84, P);
    pa4::clearButtons(e);
    S(e, 0x11c2f8, 0);
    S(e, 0x11c2f4, 0);
    S(e, 0xf46c8, 0x10);
    S(e, 0xf46c4, 0x10);
    return 0x10;
}
extern "C" uint32_t d2c_7f260(uint32_t P, const cl::Env& e) {
    using namespace pa4;
    const int32_t H = (int32_t)G(e, 0xdbc4c), W = (int32_t)G(e, 0xdbc48);
    const int32_t y = (int32_t)P16(e, P + 0xe), x = (int32_t)P16(e, P + 0xc);
    if (y >= H - 0x30) return (uint32_t)y;
    if (x >= W / 2) return (uint32_t)(W / 2);
    const uint32_t old = G(e, 0xfadc0);
    e.H(0xbf8c);
    if (G(e, 0xdd5d0) != 0x26) assertAt(e, 0x139b);
    uint32_t cur = G(e, 0xfadc0);
    if (cur == 0) {
        if (G(e, 0x113a68) != 0) return tail_7f260(e, P);
        for (uint32_t i = 0; (int32_t)i < (int32_t)G(e, 0xdd5d0); i++) {
            if (G(e, 0xfad80 + i * 4) != 1) continue;
            const uint32_t k = e.R(G(e, 0xdd538 + i * 4) + 0x40);
            if (k > 4 || k == 2 || k == 3) return tail_7f260(e, P);
            if (k == 1) {
                e.H(0xc2790, 0, 0, 0, 0, 0, 0, i, 1);
                cur = G(e, 0xfadc0);
            } else if (k == 4 && G(e, 0x119721) != 0) {
                e.H(0x47b20);
                e.Edj(0x47b20, G(e, 0x11971d));
                cur = G(e, 0xfadc0);
            }
        }
    }
    const uint32_t pl = G(e, 0x11bbfc);
    if (!pl) {
        cur = cur == 0;
        S(e, 0xfadc0, cur);
    } else if (e.R(pl) == 0 && e.H(0xa5440, 0, 0, 0, 0, 0, pl) == 0) {
        cur = cur == 0;
        S(e, 0xfadc0, cur);
    }
    if (old == 0) {
        if (cur != 0) {
            const uint32_t t = e.H(0xd0b4, 0x6FB80810u);
            e.H(0x3ccc0, t, 0x6FB80810u, 0, 0, 0, 2);
        }
    } else if (cur == 0) {
        const uint32_t t = e.H(0xd0b4, 0x6FB80810u);
        e.H(0x3cc60, t, 0x6FB80810u, 0, 0, 0, 2);
        e.H(0xbe2e0);
    }
    const int32_t Wd = (int32_t)G(e, 0xdbc48);
    if (G(e, 0xfad84) == 0 && G(e, 0xfad90) == 0) {
        if (cur == 0) {
            S(e, 0x11c418, 0);
            S(e, 0x11c414, 0);
            e.H(0xd1b0, 0, 0, 0, 0, 0, 0, 0);
            e.H(0x8a4d0, 0, 0, 0, 0, 0, G(e, 0xf9e18), G(e, 0xf9e14), 0, 0, G(e, 0x11c3c4));
        } else {
            const int32_t q = q4_7f260(Wd);
            S(e, 0x11c418, (uint32_t)q);
            S(e, 0x11c414, 2);
            e.H(0xd1b0, 0, 0, 0, 0, 0, 0, (uint32_t)q);
            const int32_t c = (int32_t)G(e, 0xf9e14), t = q4_7f260(c);
            e.H(0x8a4d0, (uint32_t)t, 0, 0, 0, 0, G(e, 0xf9e18), (uint32_t)(c + t), 0, 0, G(e, 0x11c3c4));
        }
    } else if (cur != 0) {
        S(e, 0x11c418, 0);
        S(e, 0x11c414, 3);
        e.H(0xd1b0, 0, 0, 0, 0, 0, 0, 0);
        e.H(0x8a4d0, 0, 0, 0, 0, 0, G(e, 0xf9e18), G(e, 0xf9e14), 0, 0, G(e, 0x11c3c4));
        S(e, 0x11c3dc, 0);
        S(e, 0x11c3e0, 0);
        S(e, 0x11c3f8, 0);
        S(e, 0x11c3fc, 0);
    } else {
        const int32_t q = -q4_7f260(Wd);
        S(e, 0x11c418, (uint32_t)q);
        S(e, 0x11c414, 1);
        e.H(0xd1b0, 0, 0, 0, 0, 0, 0, (uint32_t)q);
        const int32_t c = (int32_t)G(e, 0xf9e14), t = q4_7f260(c);
        e.H(0x8a4d0, (uint32_t)-t, 0, 0, 0, 0, G(e, 0xf9e18), (uint32_t)(c - t), 0, 0, G(e, 0x11c3c4));
    }
    return tail_7f260(e, P);
}
