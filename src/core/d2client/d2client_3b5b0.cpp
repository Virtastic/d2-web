// 1.13c D2Client+0x3B5B0 -- slot-pass sibling with the toggle word 0xFAD90 (object state at +0x10) and the right-hand slide tail. No
// arguments. The old toggle word is read first. While it is 0 (and the pending word 0x113A68 is clear; a set one returns the 0xBF8C
// answer) each active slot (array 0xFAD80 == 1) acts on its state: 0 nothing, 1 the request routine 0xC2790 (ECX = slot, EDX = 1, one
// zero stack word), 4 the routine 0x47B20 (EDI = the word 0x11971D) when 0x119721 is set; 2, 3 and above 4 leave at once with the state
// in EAX. The toggle word then becomes (word == 0) unless the local player (0x11BBFC) has a nonzero first dword or 0xA5440 answers
// nonzero. Old 0 and new nonzero: 0x3CCC0 (EAX = 2, handle of string 0xD0604 from 0xD0B4); old nonzero and new 0: 0x3CC60 the same
// way, then 0xBE2E0 and 0x77DA0. The slide tail (cl_slide.h) follows. The assert path exits.
#include "cl_slide.h"
extern "C" uint32_t d2c_3b5b0(const cl::Env& e) {
    using namespace pa4;
    const uint32_t ebp = G(e, 0xfad90);
    uint32_t eax = e.H(0xbf8c);
    if (G(e, 0xdd5d0) != 0x26) {
        const uint32_t r = e.H(0xbf92, 0x139b);
        e.H(0xbf80, 0x6FB8359Fu, r, 0x139b);
        eax = e.H(0x3664, 0xFFFFFFFFu);
    }
    uint32_t edx = G(e, 0xfad90);
    if (edx == 0) {
        if (G(e, 0x113a68)) return eax;
        uint32_t i = 0;
        do {
            if (G(e, 0xFAD80 + 4 * i) == 1) {
                eax = e.R(G(e, 0xDD538 + 4 * i) + 0x10);
                if (eax > 4 || eax == 2 || eax == 3) return eax;
                if (eax == 1) {
                    eax = e.H(0xc2790, 0, 0, 0, 0, 0, 0, i, 1);
                    edx = G(e, 0xfad90);
                } else if (eax == 4) {
                    if (G(e, 0x119721)) {
                        eax = e.H(0x47b20);
                        e.Ej(0x47b20, G(e, 0x11971d));
                        edx = G(e, 0xfad90);
                    }
                }
            }
            eax = G(e, 0xdd5d0);
            ++i;
        } while ((int32_t)i < (int32_t)eax);
    }
    eax = G(e, 0x11bbfc);
    if (eax) {
        if (e.R(eax) == 0) {
            eax = e.H(0xa5440);
            edx = eax;   // the recorder thunk leaves EDX = the hook's return value
            if (eax == 0) { edx = (edx == 0); S(e, 0xfad90, edx); }
        }
    } else {
        eax = (edx == 0);
        edx = eax;
        S(e, 0xfad90, edx);
    }
    const uint32_t esi = edx;
    if (ebp == 0) {
        if (edx != 0) {
            const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xD0604);
            e.H(0x3ccc0, h, 0, 0, 0, 0, 2);
        }
    } else if (edx == 0) {
        const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xD0604);
        e.H(0x3cc60, h, 0, 0, 0, 0, 2);
        e.H(0xbe2e0);
        e.H(0x77da0);
    }
    const int32_t cx = (int32_t)G(e, 0x11b828);
    const uint32_t cy = G(e, 0x11b824);
    return slide7(e, esi, cx, cy);
}
