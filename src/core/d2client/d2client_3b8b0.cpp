// 1.13c D2Client+0x3B8B0 -- a sixth sibling of the slot pass (no arguments; see 0x3B230 and 0x3B470): toggle word 0xFADA8, object
// state at +0x28, no pending-word test. While the toggle word (read first) is 0 each active slot acts on its state (0 nothing, 1 the
// request routine 0xC2790, 4 the routine 0x47B20 with EDI = the word 0x11971D when 0x119721 is set; 2, 3 and above 4 jump to the
// final test). The toggle word becomes (word == 0) unless the player's first dword is nonzero or 0xA5440 answers nonzero. Old 0
// and new 1: 0x3CCC0 (EAX = 5, handle of string 0xD0780 from 0xD0B4) and 0x11B9AC := 1, then the final test; old 0 and new 0 go
// straight to the second half of it; old 1 and new 1 return the old word; old 1 and new 0: 0x3CC60 the same way, 0x11B9AC := 0.
// Final test: a nonzero toggle word returns it. Otherwise, when 0xF16B4 is set, 0x5F9F0 runs and if the word 0x11C1B0 is not 1
// the words 0x11C1E8 and 0x11C1EC are cleared (return word - 1); when it is 1 they become screen-size/3 (0xDBC48, 0xDBC4C)
// minus the words 0x11C23C and 0x11C238 minus 0x10, and the low half of 0x55555556 * the height is returned.
#include "cl_pa4.h"
extern "C" uint32_t d2c_3b8b0(const cl::Env& e) {
    using namespace pa4;
    const uint32_t ebp = G(e, 0xfada8);
    uint32_t eax = e.H(0xbf8c);
    if (G(e, 0xdd5d0) != 0x26) {
        const uint32_t r = e.H(0xbf92, 0x139b);
        e.H(0xbf80, 0x6FB8359Fu, r, 0x139b);
        eax = e.H(0x3664, 0xFFFFFFFFu);
    }
    uint32_t edx = G(e, 0xfada8);
    enum { Final, Second, Done } to = Final;
    bool epi = true;
    if (edx == 0) {
        uint32_t i = 0;
        do {
            if (G(e, 0xFAD80 + 4 * i) == 1) {
                eax = e.R(G(e, 0xDD538 + 4 * i) + 0x28);
                if (eax > 4 || eax == 2 || eax == 3) { epi = false; break; }
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
        if (epi) edx = G(e, 0xfada8);
    }
    if (epi) {
        eax = G(e, 0x11bbfc);
        bool skip = false;
        if (eax) {
            if (e.R(eax) != 0) skip = true;
            else {
                eax = e.H(0xa5440);
                if (eax) skip = true;
            }
        }
        if (!skip) { edx = (edx == 0); S(e, 0xfada8, edx); }
        if (ebp == 0) {
            if (edx != 0) {
                const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xD0780);
                e.H(0x3ccc0, h, 0, 0, 0, 0, 5);
                S(e, 0x11b9ac, 1);
            } else to = Second;
        } else if (edx != 0) {
            return ebp;
        } else {
            const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xD0780);
            e.H(0x3cc60, h, 0, 0, 0, 0, 5);
            S(e, 0x11b9ac, 0);
        }
    }
    if (to == Final) {
        eax = G(e, 0xfada8);
        if (eax) return eax;
    }
    eax = G(e, 0xf16b4);
    if (!eax) return eax;
    e.H(0x5f9f0);
    eax = G(e, 0x11c1b0) - 1;
    if (eax != 0) {
        S(e, 0x11c1e8, 0);
        S(e, 0x11c1ec, 0);
        return eax;
    }
    const int32_t w = (int32_t)G(e, 0xdbc48), hgt = (int32_t)G(e, 0xdbc4c);
    S(e, 0x11c1e8, (uint32_t)(w / 3) - G(e, 0x11c23c) - 0x10);
    S(e, 0x11c1ec, (uint32_t)(hgt / 3) - G(e, 0x11c238) - 0x10);
    return (uint32_t)((int64_t)0x55555556 * (int64_t)hgt);
}
