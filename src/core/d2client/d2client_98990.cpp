// 1.13c D2Client+0x98990 -- window idle / close message handler: nothing to do while the quit flag at 0xFAD94 is set or the
// first check answers nonzero. Otherwise (unless the message carries 0x40000000, the build word at 0xDD5D0 must be 0x26 and
// neither of the two idle gates holds) it walks the table of 0xDD5D0 entries: an entry flagged at 0xFAD80+4i dispatches on
// the kind word at +0x14 of its record (0xD8538+4i): kind 1 calls the entry handler, kind 4 the pending-object handler with
// EDI = the id at 0x11971D, kinds 2, 3 and above 4 stop the walk. After the walk the shutdown path (quit flag set, or no
// living player) saves and closes: the client data, the 0x13349C / 0x133498 and 0xFADA8 notices, the final callee. Every
// path ends by marking the message handled; EAX is that call's answer.
#include "cl_mouse.h"
extern "C" uint32_t d2c_98990(uint32_t eax, uint32_t msg, const cl::Env& e) {
    eax = clc::G(e, 0xFAD94);
    if (eax) return eax;
    eax = e.H(0x45980);
    if (eax) return eax;
    auto tail = [&]() { return clm::done(e, msg); };
    if (e.R(msg + 0xC) & 0x40000000u) return tail();
    e.H(0xBF8C);
    if (clc::G(e, 0xDD5D0) != 0x26) {
        const uint32_t r = e.H(0xBF92, 0x139B);
        e.H(0xBF80, 0x6FB8359Fu, r, 0x139B);
        e.H(0x3664, 0xFFFFFFFFu);
    }
    if (clc::G(e, 0x11BC08) && !clc::G(e, 0xFADE0)) return tail();
    const uint32_t tick = e.H(0xCEF5C);
    if (clc::G(e, 0xFAD50) > tick) return tail();
    if (clc::G(e, 0x113A68)) return tail();
    int32_t n = (int32_t)clc::G(e, 0xDD5D0);
    if (n > 0) {
        for (int32_t i = 0; i < (int32_t)clc::G(e, 0xDD5D0) || i == 0; ++i) {
            if (clc::G(e, 0xFAD80 + (uint32_t)i * 4) == 1) {
                const uint32_t k = e.R(clc::G(e, 0xD8538 + (uint32_t)i * 4) + 0x14);
                if (k > 4 || k == 2 || k == 3) return tail();
                if (k == 1) e.H(0xC2790, 0, 0, 0, 0, 0, 0, (uint32_t)i, 1);
                else if (k == 4 && clc::G(e, 0x119721)) {
                    e.H(0x47B20);
                    e.Edj(0x47B20, clc::G(e, 0x11971D));
                }
            }
        }
    }
    const uint32_t pl = clc::G(e, 0x11BBFC);
    bool quit = false;
    if (!pl) quit = true;
    else if (e.R(pl) == 0 && e.H(0xA5440) == 0) quit = true;
    if (quit) clc::S(e, 0xFAD94, 1);
    else if (!clc::G(e, 0xFAD94)) return tail();
    e.H(0x6F5B0);
    clc::S(e, 0x11C434, 0);
    if (clc::G(e, 0x13349C) == 1) {
        clc::S(e, 0x13349C, 0);
        uint32_t r = e.H(0xD0B4, 0x6FBA51BCu);
        e.H(0x3CC60, r, 0x6FBA51BCu, 0, 0, 0, 4);
        if (!clc::G(e, 0x133498)) {
            r = e.H(0xD0B4, 0x6FBA51ECu);
            e.H(0x3CCC0, r, 0x6FBA51ECu, 0, 0, 0, 2);
            clc::S(e, 0x133498, 1);
        }
    }
    if (clc::G(e, 0xFADA8)) {
        const uint32_t r = e.H(0xD0B4, 0x6FB80780u);
        e.H(0x3CC60, r, 0x6FB80780u, 0, 0, 0, 5);
    }
    e.H(0x6EB50);
    return tail();
}
