// 1.13c D2Client+0x3AEE0 -- slot-pass sibling with the toggle word 0xFAE10 (object state at +0x90), guarded by the gate word 0x119854, the
// local player 0x11BBFC and a search of the object list (head 0x11C4D4, next at +0x30) for the entry whose kind (+4) is 7, whose owner
// (+0xC) is the player's word +0xC and whose +0x20 is 0; that entry's +8 must not be -1. No arguments. Exits with EAX 0 (or the word
// that was tested) at each failed guard, then the import 0xBF8C must answer nonzero twice. The old toggle word is read between the
// two. The slot pass, the toggle rule (new word = (old == 0) unless the player's first dword is nonzero or 0xA5440 answers nonzero)
// and the slide tail (cl_slide.h slideA) are those of 0x3BFC0; the toggle strings are 0xD099C with EAX = 3, the closing
// toggle also calls 0x8BBF0. A slot count other than 0x26 takes the assert path (exits).
#include "cl_slide.h"
extern "C" uint32_t d2c_3aee0(const cl::Env& e) {
    using namespace pa4;
    uint32_t eax = G(e, 0x119854);
    if (eax == 0) return 0;
    const uint32_t pl = G(e, 0x11bbfc);
    if (pl == 0) return eax;
    uint32_t node = G(e, 0x11c4d4);
    if (node == 0) return 0;
    const uint32_t own = e.R(pl + 0xc);
    while (!(e.R(node + 4) == 7 && e.R(node + 0xc) == own && e.R(node + 0x20) == 0)) {
        node = e.R(node + 0x30);
        if (node == 0) return 0;
    }
    eax = e.R(node + 8);
    if (eax == 0xFFFFFFFFu) return eax;
    eax = e.H(0xbf8c);
    if (eax == 0) return 0;
    const uint32_t ebx = G(e, 0xfae10);
    eax = e.H(0xbf8c);
    if (eax == 0) return 0;
    if (G(e, 0x119854) == 0) return eax;
    if (G(e, 0xdd5d0) != 0x26) {
        const uint32_t r = e.H(0xbf92, 0x139b);
        e.H(0xbf80, 0x6FB8359Fu, r, 0x139b);
        eax = e.H(0x3664, 0xFFFFFFFFu);
    }
    uint32_t edx = G(e, 0xfae10);
    if (edx == 0) {
        if (G(e, 0x113a68)) return G(e, 0x113a68);
        uint32_t i = 0;
        do {
            if (G(e, 0xFAD80 + 4 * i) == 1) {
                eax = e.R(G(e, 0xDD538 + 4 * i) + 0x90);
                if (eax > 4 || eax == 2 || eax == 3) return eax;
                if (eax == 1) {
                    eax = e.H(0xc2790, 0, 0, 0, 0, 0, 0, i, 1);
                    edx = G(e, 0xfae10);
                } else if (eax == 4) {
                    if (G(e, 0x119721)) {
                        eax = e.H(0x47b20);
                        e.Ej(0x47b20, G(e, 0x11971d));
                        edx = G(e, 0xfae10);
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
            if (eax == 0) { edx = (edx == 0); S(e, 0xfae10, edx); }
        }
    } else {
        edx = (edx == 0);
        S(e, 0xfae10, edx);
    }
    const uint32_t esi = edx;
    if (ebx == 0) {
        if (edx != 0) {
            const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xD099C);
            e.H(0x3ccc0, h, 0, 0, 0, 0, 3);
        }
    } else if (edx == 0) {
        const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xD099C);
        e.H(0x3cc60, h, 0, 0, 0, 0, 3);
        e.H(0x8bbf0);
    }
    const int32_t cx = (int32_t)G(e, 0x11b828);
    const uint32_t cy = G(e, 0x11b824);
    return slideA(e, esi, cx, cy);
}
