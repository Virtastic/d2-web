// 1.13c D2Client+0x3B230 -- per-frame pass over the 0x26 panel slots (no arguments; EAX is whatever the last step left).
// With the pending word (0x113A68) clear, every slot whose active word (array 0xFAD80) is exactly 1 looks at its object's
// state (+0x34 of the pointer in array 0xDD538): 0 does nothing, 1 calls the request routine 0xC2790 (ECX = slot, EDX = 1,
// one zero stack word), 4 calls 0x47B20 (EDI = the word at 0x11971D) when the flag at 0x119721 is set, and 2, 3 or above 4
// leave the whole pass at once. When the pass ran to its end the ready word (0xFADB4) becomes 1 if there is no local player
// or the player's first dword is 0 and 0xA5440 answers 0. A slot count other than 0x26 takes the assert path (line 0x139B).
#include "cl_pa4.h"
extern "C" uint32_t d2c_3b230(const cl::Env& e) {
    using namespace pa4;
    uint32_t eax = e.H(0xbf8c);
    if (G(e, 0xdd5d0) != 0x26) {
        const uint32_t r = e.H(0xbf92, 0x139b);
        e.H(0xbf80, 0x6FB8359Fu, r, 0x139b);
        eax = e.H(0x3664, 0xFFFFFFFFu);
    }
    eax = G(e, 0x113a68);
    if (eax) return eax;
    uint32_t i = 0;
    do {
        if (G(e, 0xFAD80 + 4 * i) == 1) {
            const uint32_t obj = G(e, 0xDD538 + 4 * i);
            eax = e.R(obj + 0x34);
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
    eax = G(e, 0x11bbfc);
    if (eax) {
        if (e.R(eax) != 0) return eax;
        eax = e.H(0xa5440);
        if (eax) return eax;
    }
    S(e, 0xFADB4, 1);
    return eax;
}
