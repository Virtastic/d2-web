// 1.13c D2Client+0x3B470 -- the third sibling of the slot pass (see 0x3B230 and 0x3BEB0): object state at +0xC, toggle word 0xFAD8C,
// and every exit stores 0 in the word 0x11C420. The old toggle word is read first. While it is 0 (and the pending word 0x113A68
// is clear) each active slot (array 0xFAD80 == 1) acts on its state: 0 nothing, 1 the request routine 0xC2790 (ECX = slot, EDX = 1,
// one zero stack word), 4 the routine 0x47B20 (EDI = the word 0x11971D) when 0x119721 is set; 2, 3 and above 4 leave at once
// (clearing 0x11C420). With the toggle word nonzero the loop is skipped. The toggle word then becomes (word == 0) unless the local
// player (0x11BBFC) has a nonzero first dword or 0xA5440 answers nonzero. Then: old word 0 and new word 1 calls 0x3CCC0 (EAX = 1)
// with the handle of string 0xD061C from 0xD0B4; old word 1 and new word 0 calls 0x3CC60 the same way; the 0x3CCC0 branch stores
// the old word (0) and the rest store 0 in 0x11C420. A slot count other than 0x26 takes the assert path (exits).
#include "cl_pa4.h"
extern "C" uint32_t d2c_3b470(const cl::Env& e) {
    using namespace pa4;
    const uint32_t ebp = G(e, 0xfad8c);
    uint32_t eax = e.H(0xbf8c);
    if (G(e, 0xdd5d0) != 0x26) {
        const uint32_t r = e.H(0xbf92, 0x139b);
        e.H(0xbf80, 0x6FB8359Fu, r, 0x139b);
        eax = e.H(0x3664, 0xFFFFFFFFu);
    }
    uint32_t edx = G(e, 0xfad8c);
    if (edx == 0) {
        eax = G(e, 0x113a68);
        if (eax) { S(e, 0x11c420, 0); return eax; }
        uint32_t i = 0;
        do {
            if (G(e, 0xFAD80 + 4 * i) == 1) {
                eax = e.R(G(e, 0xDD538 + 4 * i) + 0xc);
                if (eax > 4 || eax == 2 || eax == 3) { S(e, 0x11c420, 0); return eax; }
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
        edx = G(e, 0xfad8c);
    }
    eax = G(e, 0x11bbfc);
    if (eax) {
        if (e.R(eax) == 0) {
            eax = e.H(0xa5440);
            if (eax == 0) { edx = (edx == 0); S(e, 0xfad8c, edx); }
        }
    } else {
        eax = (edx == 0);
        edx = eax;
        S(e, 0xfad8c, edx);
    }
    if (ebp == 0) {
        if (edx != 0) {
            const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xD061C);
            eax = e.H(0x3ccc0, h, 0, 0, 0, 0, 1);
            S(e, 0x11c420, ebp);
            return eax;
        }
    } else if (edx == 0) {
        const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xD061C);
        eax = e.H(0x3cc60, h, 0, 0, 0, 0, 1);
    }
    S(e, 0x11c420, 0);
    return eax;
}
