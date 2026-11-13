// 1.13c D2Client+0x3BC10 -- slot-pass sibling with the toggle word 0xFAD94 (object state at +0x14) that also drives two text strips
// and a timer gate. No arguments. Returns the word 0x11C3A4 while it is set. Otherwise the old toggle word is saved and the import
// 0xBF8C called (a slot count other than 0x26 takes the assert path, which exits). With the old word 0: the word 0x11BC08 set while
// 0xFADE0 is clear returns 0; a tick count (GetTickCount, slot 0xCEF5C) below the word 0xFAD50 returns the tick; the pending word
// 0x113A68 returns itself; then each active slot (array 0xFAD80 == 1) acts on its state: 0 nothing, 1 the request routine 0xC2790
// (ECX = slot, EDX = 1, one zero stack word), 4 the routine 0x47B20 (EDI = the word 0x11971D) when 0x119721 is set; 2, 3 and above
// 4 return the state. The new toggle word is (EDX == 0), EDX being the word re-read after the pass (the old one when the pass
// did not run); with the local player (0x11BBFC) present and its first dword 0 the routine 0xA5440 is called first and EDX is left
// as the hooked call leaves it. Old 0 and new nonzero: 0x6F5B0, 0x11C434 := 0; when 0x12349C is 1 it becomes 0, 0x3CC60
// (EAX = 4, string 0xF51BC) runs and, with 0x123498 clear, 0x3CCC0 (EAX = 2, string 0xF51EC) runs and 0x123498 := 1; with 0xFADA8
// set 0x3CC60 (EAX = 5, string 0xD0780); the result is the tail call 0x6EB50. Old nonzero and new 0: 0x6F4A0; with the nine words
// 0xFADB0, 0xFADDC, 0xFADE4, 0xFADE8, 0xFADEC, 0xFADF0, 0xFADF4, 0xFADF8, 0xFAE00 all clear 0x11C434 := 1 and, when 0x12349C is
// 0, it becomes 1, 0x3CC60 (EAX = 2, string 0xF51EC) runs if 0x123498 is set (and clears it) and 0x3CCC0 (EAX = 4, string 0xF51BC);
// with 0xFADA8 set 0x3CCC0 (EAX = 5, string 0xD0780). Old 0 and new 0 return 0, old and new nonzero return the new word.
#include "cl_pa4.h"
extern "C" uint32_t d2c_3bc10(const cl::Env& e) {
    using namespace pa4;
    uint32_t eax = G(e, 0x11c3a4);
    if (eax) return eax;
    const uint32_t old = G(e, 0xfad94);
    e.H(0xbf8c);
    if (G(e, 0xdd5d0) != 0x26) {
        const uint32_t r = e.H(0xbf92, 0x139b);
        e.H(0xbf80, 0x6FB8359Fu, r, 0x139b);
        eax = e.H(0x3664, 0xFFFFFFFFu);
    }
    uint32_t edx = G(e, 0xfad94);
    if (edx == 0) {
        if (G(e, 0x11bc08) != 0 && G(e, 0xfade0) == 0) return 0;
        eax = e.H(0xcef5c);
        if (G(e, 0xfad50) > eax) return eax;
        eax = G(e, 0x113a68);
        if (eax) return eax;
        eax = G(e, 0xdd5d0);
        uint32_t i = 0;
        if ((int32_t)eax > 0) {
            do {
                if (G(e, 0xFAD80 + 4 * i) == 1) {
                    eax = e.R(G(e, 0xDD538 + 4 * i) + 0x14);
                    if (eax > 4 || eax == 2 || eax == 3) return eax;
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
        }
        edx = G(e, 0xfad94);
    }
    eax = G(e, 0x11bbfc);
    if (eax && e.R(eax) == 0) eax = e.H(0xa5440);
    eax = (edx == 0);
    S(e, 0xfad94, eax);
    if (old == 0) {
        if (eax == 0) return 0;
        e.H(0x6f5b0);
        S(e, 0x11c434, 0);
        if (G(e, 0x12349c) == 1) {
            S(e, 0x12349c, 0);
            uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xF51BC);
            eax = e.H(0x3cc60, h, 0, 0, 0, 0, 4);
            if (G(e, 0x123498) == 0) {
                h = e.H(0xd0b4, 0xB2000000u | 0xF51EC);
                e.H(0x3ccc0, h, 0, 0, 0, 0, 2);
                S(e, 0x123498, 1);
            }
        }
        if (G(e, 0xfada8)) {
            const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xD0780);
            e.H(0x3cc60, h, 0, 0, 0, 0, 5);
        }
        return e.H(0x6eb50);
    }
    if (eax != 0) return eax;
    eax = e.H(0x6f4a0);
    static const uint32_t fl[9] = {0xfadb0, 0xfaddc, 0xfade4, 0xfade8, 0xfadec, 0xfadf0, 0xfadf4, 0xfadf8, 0xfae00};
    bool any = false;
    for (uint32_t f : fl) {
        eax = G(e, f);
        if (eax) { any = true; break; }
    }
    if (!any) {
        eax = G(e, 0x12349c);
        S(e, 0x11c434, 1);
        if (eax == 0) {
            eax = G(e, 0x123498);
            S(e, 0x12349c, 1);
            if (eax) {
                const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xF51EC);
                eax = e.H(0x3cc60, h, 0, 0, 0, 0, 2);
                S(e, 0x123498, 0);
            }
            const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xF51BC);
            eax = e.H(0x3ccc0, h, 0, 0, 0, 0, 4);
        }
    }
    eax = G(e, 0xfada8);
    if (eax) {
        const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xD0780);
        eax = e.H(0x3ccc0, h, 0, 0, 0, 0, 5);
    }
    return eax;
}
