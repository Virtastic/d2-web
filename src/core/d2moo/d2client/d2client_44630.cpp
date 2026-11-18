// 1.13c D2Client+0x44630 -- right-button-down message: marks the right button state (0x10), clears the right latch
// and raises its held word. With the game view active and a live player of type 0 the click (5, x, y, key flags)
// goes to the click router, whose answer becomes the right latch; the message is finished.
#include "cl_pa4.h"
extern "C" uint32_t d2c_44630(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    const uint32_t on = G(e, 0x11c3a0);
    S(e, 0xf46c8, 0x10);
    S(e, 0x11c3e0, 0);
    S(e, 0x11c3fc, 1);
    S(e, 0x11c3e4, 0);
    S(e, 0x11c2f8, 0);
    if (!on) return 0;
    const uint32_t pl = G(e, 0x11bbfc);
    if (pl && e.R(pl) == 0) S(e, 0x11c3e0, e.H(0x1bf20, 5, P16(e, msg + 0xc), P16(e, msg + 0xe), P16(e, msg + 8)));
    return fin(e, msg);
}
