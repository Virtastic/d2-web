// 1.13c D2Client+0x65C70 -- per-frame client step: refresh, build check, run the per-slot handlers whose flag is raised, then open or close the world panel
// state on the change of the word at 0xFADAC.
#include "cl_cmd.h"
extern "C" uint32_t d2c_65c70(uint32_t, const cl::Env& e) {
    using namespace clc;
    e.H(0x656b0, 0);
    const uint32_t old = G(e, 0xFADAC);
    e.H(0xbf8c);
    if (G(e, 0xDD5D0) != 0x26) {
        const uint32_t r = e.H(0xbf92, 0x139b);
        e.H(0xbf80, 0x6FB8359Fu, r, 0x139b);
        e.H(0x3664, 0xFFFFFFFFu);
    }
    uint32_t eax = G(e, 0x113A68);
    if (eax) return eax;
    int32_t i = 0;
    do {
        if (G(e, 0xFAD80 + 4 * (uint32_t)i) == 1) {
            const uint32_t t = e.R(G(e, 0xDD538 + 4 * (uint32_t)i) + 0x2C);
            if (t > 4 || t == 2 || t == 3) return t;
            if (t == 1) e.H(0xc2790, 0, 0, 0, 0, 0, 0, (uint32_t)i, 1);
            else if (t == 4 && G(e, 0x119721)) {
                e.H(0x47b20);
                e.Edj(0x47b20, G(e, 0x11971D));
            }
        }
        eax = G(e, 0xDD5D0);
        ++i;
    } while (i < (int32_t)eax);
    uint32_t cur;
    const uint32_t pl = G(e, kPlayer);
    if (!pl || (e.R(pl) == 0 && e.H(0xa5440) == 0)) { cur = 1; S(e, 0xFADAC, 1); }
    else cur = G(e, 0xFADAC);
    if (old == 0) {
        if (cur == 0) return e.H(0x11f80);
        e.H(0x12dd0, 1);
        return e.H(0xbe920);
    }
    if (cur) return cur;
    if (e.H(0x42090) || e.H(0x420e0)) {
        const uint32_t r = e.H(0xd0b4, 0x6FB80774u);
        e.H(0x3cc60, r, 0, 0, 0, 0, 1);
    }
    const uint32_t r = e.H(0xd0b4, 0x6FB806D8u);
    e.H(0x3cc60, r, 0, 0, 0, 0, 0xD);
    e.H(0x12dd0, 0);
    return e.H(0x11f80);
}
