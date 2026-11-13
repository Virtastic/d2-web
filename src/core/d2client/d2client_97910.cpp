// 1.13c D2Client+0x97910 -- mouse-move listener of the two-panel screen (stdcall msg, ret 4). Does nothing (and leaves the message untouched)
// unless the pointer (x = [0x11B828], y = [0x11B824]) is inside 0..screenW/2 by 0..screenH-0x31. Inside, when the expansion flag (0xBF8C) and the
// skill tree flag [0x119854] are set the screen mode word [0xDD5D0] must be 0x26 (else the 0x139B assertion): the pending flag [0xFAE14] clears and,
// if it was set, the string 0xD09C0 is looked up through 0xD0B4 and handed to 0x3CC60 (EAX = 2); then the panel origin is set -- both side panels
// closed ([0xFAD84], [0xFAD90]): origin 0 and 0xD1B0(0), otherwise origin -(screenW/4) with 0xD1B0 of that and the window shifted by a quarter
// of [0xF9E14] -- and 0x8A4D0 is called (EAX = [0xF9E18], ECX = x, EDX = 0, ESI = [0x11C3C4], stack word = y). The four button latches clear and the
// message is finished.
#include "cl_pa4.h"
extern "C" uint32_t d2c_97910(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    const auto q4 = [](uint32_t v) { return (uint32_t)(((int32_t)v + (int32_t)((((int32_t)v) >> 31) & 3)) >> 2); };
    const int32_t px = (int32_t)G(e, 0x11b828), py = (int32_t)G(e, 0x11b824);
    const int32_t w = (int32_t)G(e, 0xdbc48), h = (int32_t)G(e, 0xdbc4c);
    if (px < 0 || px > (w - (w >> 31)) >> 1 || py < 0 || py > h - 0x31) return 0;
    const uint32_t pending = G(e, 0xfae14);
    if (e.H(0xbf8c) != 0 && G(e, 0x119854) != 0) {
        if (G(e, 0xdd5d0) != 0x26) { assertAt(e, 0x139b); return 0; }
        S(e, 0xfae14, 0);
        if (pending) {
            const uint32_t s = e.H(0xd0b4, kBase + 0xd09c0);
            e.H(0x3cc60, s, kBase + 0xd09c0, 0, 0, 0, 2);
        }
        uint32_t x, y;
        if (G(e, 0xfad84) == 0 && G(e, 0xfad90) == 0) {
            S(e, 0x11c418, 0);
            S(e, 0x11c414, 0);
            e.H(0xd1b0, 0, 0, 0, 0, 0, 0, 0);
            x = G(e, 0xf9e14);
            y = 0;
        } else {
            const uint32_t v = 0u - q4(G(e, 0xdbc48));
            S(e, 0x11c418, v);
            S(e, 0x11c414, 1);
            e.H(0xd1b0, 0, 0, 0, 0, 0, 0, v);
            const uint32_t f = G(e, 0xf9e14), qq = q4(f);
            x = f - qq;
            y = 0u - qq;
        }
        e.H(0x8a4d0, y, 0, 0, 0, 0, G(e, 0xf9e18), x, 0, 0, G(e, 0x11c3c4));
    }
    clearButtons(e);
    return fin(e, msg);
}
