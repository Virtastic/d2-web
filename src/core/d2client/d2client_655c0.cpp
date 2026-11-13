// 1.13c D2Client+0x655C0 -- button-up handler of the tabbed panel: when a press is pending and the cursor is still
// over the pressed tab (the hit test with EBX = y) the tab is activated. The button states, latches and the
// panel's two press words are reset and the message finished either way.
#include "cl_pa4.h"
extern "C" uint32_t d2c_655c0(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    const uint32_t y = P16(e, msg + 0xe);
    if (G(e, 0x11c068) && e.H(0x64080, 0, 0, 0, 0, 0, 0, 0, 0, y) == G(e, 0x11c058)) e.H(0x65180);
    S(e, 0xf46c8, 0x10);
    S(e, 0xf46c4, 0x10);
    clearButtons(e);
    S(e, 0x11c2f8, 0);
    S(e, 0x11c2f4, 0);
    S(e, 0x11c068, 0);
    S(e, 0x11c06c, 0);
    return fin(e, msg);
}
