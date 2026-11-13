// 1.13c D2Client+0x3BEB0 -- the sibling of 0x3B230 for the second slot pass: same 0x26 slots (active words 0xFAD80, object
// pointers 0xDD538) but the object state is the dword at +0x60 and the pass toggles the word 0xFADE0 instead of setting 0xFADB4.
// The old toggle word is read first. While it is 0 (and the pending word 0x113A68 is clear) each active slot acts on its state:
// 0 nothing, 1 the request routine 0xC2790 (ECX = slot, EDX = 1, one zero stack word), 4 the routine 0x47B20 (EDI = the word
// 0x11971D) when 0x119721 is set; 2, 3 and anything above 4 leave the pass with the state in EAX. When the toggle word was
// nonzero the loop is skipped. Then the toggle word becomes (word == 0) unless a local player (0x11BBFC) starts with a nonzero
// dword or 0xA5440 answers nonzero (then it is left alone). A toggle going from 0 to 1 ends in a tail jump to 0x7C770 and one going
// from 1 to 0 to 0x7C730; the other combinations return with EAX as left. A slot count other than 0x26 takes the assert path (exits).
#include "cl_pa4.h"
extern "C" uint32_t d2c_3beb0(const cl::Env& e) {
    using namespace pa4;
    const uint32_t ebp = G(e, 0xfade0);
    uint32_t eax = e.H(0xbf8c);
    if (G(e, 0xdd5d0) != 0x26) {
        const uint32_t r = e.H(0xbf92, 0x139b);
        e.H(0xbf80, 0x6FB8359Fu, r, 0x139b);
        eax = e.H(0x3664, 0xFFFFFFFFu);
    }
    uint32_t edx = G(e, 0xfade0);
    if (edx == 0) {
        eax = G(e, 0x113a68);
        if (eax) return eax;
        uint32_t i = 0;
        do {
            if (G(e, 0xFAD80 + 4 * i) == 1) {
                eax = e.R(G(e, 0xDD538 + 4 * i) + 0x60);
                if (eax > 4) return eax;
                if (eax == 2 || eax == 3) return eax;
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
        edx = G(e, 0xfade0);
    }
    eax = G(e, 0x11bbfc);
    if (eax) {
        if (e.R(eax) == 0) {
            eax = e.H(0xa5440);
            if (eax == 0) { edx = (edx == 0); S(e, 0xfade0, edx); }
        }
    } else {
        eax = (edx == 0);
        edx = eax;
        S(e, 0xfade0, edx);
    }
    if (ebp == 0) {
        if (edx == 0) return eax;
        return e.H(0x7c770);
    }
    if (edx != 0) return eax;
    return e.H(0x7c730);
}
