// 1.13c D2Client+0x448E0 -- left-button-down message: marks the left button state (4). With the game view active
// the left latch is raised and, for a live player of type 0, the click (0, x, y, modifier flags: 8 for either
// shift latch, 4 for the third) goes to the click router, whose answer becomes the left latch.
#include "cl_pa4.h"
extern "C" uint32_t d2c_448e0(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    const uint32_t on = G(e, 0x11c3a0);
    S(e, 0xf46c4, 4);
    S(e, 0x11c3e4, 0);
    S(e, 0x11c2f8, 0);
    if (!on) return 0;
    const uint32_t pl = G(e, 0x11bbfc);
    S(e, 0x11c3dc, 1);
    uint32_t fl = 0;
    if (G(e, 0x11c3e8)) fl = 8;
    if (G(e, 0x11c3ec)) fl |= 8;
    if (G(e, 0x11c3f0)) fl |= 4;
    if (pl && e.R(pl) == 0) S(e, 0x11c3dc, e.H(0x1bf20, 0, P16(e, msg + 0xc), P16(e, msg + 0xe), fl));
    return fin(e, msg);
}
