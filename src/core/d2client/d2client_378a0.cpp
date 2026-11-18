// 1.13c D2Client+0x378A0 -- releases two cached resources (one ignored stack word, ret 4): the first slot (0xFAE04) is
// cleared and, if it held something, the release path 0x358E0 runs and 0x3CC60 is told to free through the handle the
// import 0xD0B4 returns for string 0xD0924 with code 5 (EAX); then, when the flag 0x11C44C is set, the same for slot
// 0xFADD4 (string 0xD0864, code 2), and the flag is cleared. Each slot clear is preceded by 0xBF8C and a check that the
// global at 0xDD5D0 equals 0x26 (else the assert path, line 0x139B). The compiler shares ONE assert block: a failed
// first check falls out of it into the second slot's clear with the FIRST slot's old value in ESI, skipping the first
// release and the flag test. EAX at the return is whatever the last call (or the flag load) left.
#include "cl_pa4.h"
extern "C" uint32_t d2c_378a0(uint32_t /*ignored*/, const cl::Env& e) {
    using namespace pa4;
    uint32_t esi = G(e, 0xFAE04);
    uint32_t eax = e.H(0xbf8c);
    bool toSecond = false;
    if (G(e, 0xdd5d0) != 0x26) {
        const uint32_t r = e.H(0xbf92, 0x139b);
        e.H(0xbf80, 0x6FB8359Fu, r, 0x139b);
        eax = e.H(0x3664, 0xFFFFFFFFu);
        toSecond = true;
    } else {
        S(e, 0xFAE04, 0);
        if (esi) {
            e.H(0x358e0);
            const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xD0924);
            eax = e.H(0x3cc60, h, 0, 0, 0, 0, 5);
        }
    }
    if (!toSecond) {
        eax = G(e, 0x11c44c);
        if (!eax) return eax;
        esi = G(e, 0xFADD4);
        eax = e.H(0xbf8c);
        if (G(e, 0xdd5d0) != 0x26) {
            const uint32_t r = e.H(0xbf92, 0x139b);
            e.H(0xbf80, 0x6FB8359Fu, r, 0x139b);
            eax = e.H(0x3664, 0xFFFFFFFFu);
        }
    }
    S(e, 0xFADD4, 0);
    if (esi) {
        const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xD0864);
        eax = e.H(0x3cc60, h, 0, 0, 0, 0, 2);
    }
    S(e, 0x11c44c, 0);
    return eax;
}
