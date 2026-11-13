// 1.13c D2Client+0x3B300 -- a fourth sibling of the slot pass (see 0x3B230, 0x3B470, 0x3BEB0) gated on the local player: nothing
// (EAX 0) unless the player (0x11BBFC) is set, 0xC262(player+0x60, 8) finds an item and 0xC1CC(item, 0x13) is nonzero. Then the
// same pass with the object state at +0x7C and the toggle word 0xFADFC (see 0x3BEB0 for the pass; here the end differs): old word 0
// and new word 1 runs 0xBFA10 with EDI = the string 0xD0900, then 0x3CCC0 with EAX = 2 and the handle of string 0xDBE6C from
// 0xD0B4; old word 1 and new word 0 calls 0x3CC60 the same way; the other combinations return with EAX as left.
#include "cl_pa4.h"
extern "C" uint32_t d2c_3b300(const cl::Env& e) {
    using namespace pa4;
    uint32_t eax = G(e, 0x11bbfc);
    if (!eax) return eax;
    eax = e.H(0xc262, e.R(eax + 0x60), 8);
    if (!eax) return eax;
    eax = e.H(0xc1cc, eax, 0x13);
    if (!eax) return eax;
    const uint32_t ebp = G(e, 0xfadfc);
    eax = e.H(0xbf8c);
    if (G(e, 0xdd5d0) != 0x26) {
        const uint32_t r = e.H(0xbf92, 0x139b);
        e.H(0xbf80, 0x6FB8359Fu, r, 0x139b);
        eax = e.H(0x3664, 0xFFFFFFFFu);
    }
    uint32_t edx = G(e, 0xfadfc);
    if (edx == 0) {
        eax = G(e, 0x113a68);
        if (eax) return eax;
        uint32_t i = 0;
        do {
            if (G(e, 0xFAD80 + 4 * i) == 1) {
                eax = e.R(G(e, 0xDD538 + 4 * i) + 0x7c);
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
        edx = G(e, 0xfadfc);
    }
    eax = G(e, 0x11bbfc);
    if (eax) {
        if (e.R(eax) == 0) {
            eax = e.H(0xa5440);
            edx = eax;   // the recorder thunk leaves EDX = the hook's return value
            if (eax == 0) { edx = (edx == 0); S(e, 0xfadfc, edx); }
        }
    } else {
        eax = (edx == 0);
        edx = eax;
        S(e, 0xfadfc, edx);
    }
    if (ebp == 0) {
        if (edx != 0) {
            e.H(0xbfa10);
            // EDI = the nine-dword constant block at 0xD0900 (read-only data); the capture logs it as a `D` record
            uint32_t blk[9];
            for (uint32_t j = 0; j < 9; j++) blk[j] = G(e, 0xD0900 + 4 * j);
            if (e.blk) e.blk(e.ctx, 0xbfa10, (uint32_t)(uintptr_t)blk, 9);
            const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xDBE6C);
            return e.H(0x3ccc0, h, 0, 0, 0, 0, 2);
        }
    } else if (edx == 0) {
        const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xDBE6C);
        return e.H(0x3cc60, h, 0, 0, 0, 0, 2);
    }
    return eax;
}
