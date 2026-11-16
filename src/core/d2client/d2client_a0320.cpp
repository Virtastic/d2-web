// 1.13c D2Client+0xA0320 -- left-button-down handler of the item-use slot (stdcall, one stack word = the message record P, ret 4, void; EAX
// modelled). x = u16[P+0xC], y = u16[P+0xE]. P[+0x18] = 1, P[+0x1C] = 0, 0xBE84(P). The tick (import 0xCEF5C) less the stamp at 0x10B228 below 400
// returns. Else the stamp := a second tick and the mode at 0x10B214 picks 0x9BE70 (1) or 0x8CED0 (2): a nonzero answer returns, zero runs
// 0x9F7B0(x, y); other modes fall on. Then with the player (0x11BBFC) set, the unit 0xC202(player+0x60) set, x in [0x7B, 0xD3] and y in
// [0x6A, 0xDC], the unit 0xC1BA(unit+4) set and 0xC1A8(unit, 0x10, 0x210, "..\Source\D2Client\UI\useitem.cpp") nonzero: with 0x10B210 = 1 the
// dword at 0x119725 = 0x9A / 0x1FF / 0x200 asks 0xC7CC / 0xC7D2 / 0xC7C6 (EAX the unit, the first nonzero answer runs 0x9F950 and returns); with
// 0x10B210 = 0 the dword at +0x80 of the 0xC1BA record = 0x20747368 runs 0x9F950 (EAX = the unit). Every other way ends in 0x88A70(0,0,0,0) with EBX = 3.
#include "cl_pa4.h"
extern "C" uint32_t d2c_a0320(uint32_t P, const cl::Env& e) {
    using namespace pa4;
    const uint32_t x = P16(e, P + 0xc), y = P16(e, P + 0xe);
    uint32_t eax = fin(e, P);
    const uint32_t t1 = e.H(0xcef5c);
    const uint32_t d = t1 - G(e, 0x10b228);
    if (d < 400) return d;
    S(e, 0x10b228, e.H(0xcef5c));
    const uint32_t md = G(e, 0x10b214);
    eax = md;
    if (md == 1 || md == 2) {
        eax = md == 1 ? e.H(0x9be70) : e.H(0x8ced0);
        if (eax != 0) return eax;
        eax = e.H(0x9f7b0, x, y);
    }
    const uint32_t pl = G(e, 0x11bbfc);
    if (!pl) return pl;
    const uint32_t un = e.H(0xc202, e.R(pl + 0x60));
    if (!un) return 0;
    if ((int32_t)x < 0x7b || (int32_t)x > 0xd3 || (int32_t)y < 0x6a || (int32_t)y > 0xdc) return un;
    const uint32_t rec = e.H(0xc1ba, e.R(un + 4));
    if (!rec) return 0;
    bool use = false;
    if (e.H(0xc1a8, un, 0x10, 0x210, 0x6FB84538u) != 0) {
        const uint32_t m = G(e, 0x10b210);
        if (m == 1) {
            const uint32_t k = G(e, 0x119725);
            if (k == 0x9a && e.H(0xc7cc, un) != 0) use = true;
            else if (k == 0x1ff && e.H(0xc7d2, un) != 0) use = true;
            else if (k == 0x200 && e.H(0xc7c6, un) != 0) use = true;
        } else if (m == 0 && e.R(rec + 0x80) == 0x20747368u) {
            use = true;
        }
    }
    if (use) return e.H(0x9f950, 0, 0, 0, 0, 0, un);
    return e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 3);
}
