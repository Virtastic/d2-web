// 1.13c D2Client+0x94E80 -- mouse move in screen state 0xC or 0xD: stores the position; inside the rect at 0x10B2F8 or
// when the hit test answers nonzero the move body runs, otherwise the hover words are cleared and the cursor reset.
#include "cl_mouse.h"
extern "C" uint32_t d2c_94e80(uint32_t eax, uint32_t msg, const cl::Env& e) {
    eax = clc::G(e, 0x11BC34);
    if (eax != 0xC && eax != 0xD) return eax;
    const uint32_t x = clm::X(e, msg), y = clm::Y(e, msg);
    clm::pos(e, x, y);
    if (!clm::in(e, 0x10B2F8, x, y)) {
        eax = e.H(0x8C4C0);
        if (!eax) {
            clm::hov(e);
            return clm::reset(e);
        }
    }
    return e.H(0x94AA0, msg);
}
