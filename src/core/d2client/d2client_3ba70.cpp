// 1.13c D2Client+0x3BA70 -- a fifth sibling of the slot pass (see 0x3B230, 0x3B470, 0x3BEB0) with a different shape. ECX is only the
// initial value of the one stack local. Nothing happens (EAX as left) when the word 0x11C3A4 is set, there is no local player
// (0x11BBFC) or the player's dword +0x10 is 0x11. Otherwise the pass runs with the object state at +0x84 and the toggle word
// 0xFAE04: while the toggle word is 0 and the pending word 0x113A68 is clear each active slot acts on its state (0 nothing, 1 the
// request routine 0xC2790, 4 the routine 0x47B20 with EDI = the word 0x11971D when 0x119721 is set; 2, 3 and above 4 stop the
// loop). The toggle word becomes (word == 0) unless the player's first dword is nonzero or 0xA5440 answers nonzero. Old 0, new 1
// calls 0x3CCC0 (EAX = 5, handle of string 0xD0924 from 0xD0B4); old 1, new 0 runs 0x358E0 then 0x3CC60 the same way. The
// tail asks 0xBE8A (string pair 0xD3588 / 0xD6864, 1, &local) and returns the local if that answers nonzero and the local is
// nonzero; otherwise the local becomes 1 and 0xBEF6 is called with (0xD3588, 0xD6864, 1, 1) and its result returned.
// A slot count other than 0x26 takes the assert path (exits).
#include "cl_pa4.h"
extern "C" uint32_t d2c_3ba70(uint32_t ecx, const cl::Env& e) {
    using namespace pa4;
    uint32_t loc = ecx;
    uint32_t eax = G(e, 0x11c3a4);
    if (eax) return eax;
    eax = G(e, 0x11bbfc);
    if (!eax) return eax;
    if (e.R(eax + 0x10) == 0x11) return eax;
    const uint32_t ebp = G(e, 0xfae04);
    eax = e.H(0xbf8c);
    if (G(e, 0xdd5d0) != 0x26) {
        const uint32_t r = e.H(0xbf92, 0x139b);
        e.H(0xbf80, 0x6FB8359Fu, r, 0x139b);
        eax = e.H(0x3664, 0xFFFFFFFFu);
    }
    uint32_t edx = G(e, 0xfae04);
    bool tail = false;
    if (edx == 0) {
        eax = G(e, 0x113a68);
        if (eax) tail = true;
        else {
            uint32_t i = 0;
            do {
                if (G(e, 0xFAD80 + 4 * i) == 1) {
                    eax = e.R(G(e, 0xDD538 + 4 * i) + 0x84);
                    if (eax > 4 || eax == 2 || eax == 3) { tail = true; break; }
                    if (eax == 1) {
                        eax = e.H(0xc2790, 0, 0, 0, 0, 0, 0, i, 1);
                    } else if (eax == 4) {
                        eax = G(e, 0x119721);
                        if (eax) {
                            eax = e.H(0x47b20);
                            e.Ej(0x47b20, G(e, 0x11971d));
                        }
                    }
                }
                eax = G(e, 0xdd5d0);
                ++i;
            } while ((int32_t)i < (int32_t)eax);
            if (!tail) edx = G(e, 0xfae04);
        }
    }
    if (!tail) {
        eax = G(e, 0x11bbfc);
        if (eax) {
            if (e.R(eax) == 0) {
                eax = e.H(0xa5440);
                if (eax == 0) { edx = (edx == 0); S(e, 0xfae04, edx); }
            }
        } else {
            eax = (edx == 0);
            edx = eax;
            S(e, 0xfae04, edx);
        }
        if (ebp == 0) {
            if (edx != 0) {
                const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xD0924);
                e.H(0x3ccc0, h, 0, 0, 0, 0, 5);
            }
        } else if (edx == 0) {
            e.H(0x358e0);
            const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xD0924);
            e.H(0x3cc60, h, 0, 0, 0, 0, 5);
        }
    }
    const uint32_t la = (uint32_t)(uintptr_t)&loc;
    eax = e.H(0xbe8a, 0x6FB83588u, 0x6FB86864u, 1, la);
    if (eax) {
        eax = e.R(la);
        if (eax) return eax;
    }
    loc = 1;
    return e.H(0xbef6, 0x6FB83588u, 0x6FB86864u, 1, 1);
}
