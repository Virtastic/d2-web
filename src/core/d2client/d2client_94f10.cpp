// 1.13c D2Client+0x94F10 -- mouse move in screen state 0xB: stores the position; inside the rect at 0x10B2F8, inside the
// open-bounds rect at 0x10B28C (x strictly inside) or when the hit test answers nonzero the move body runs, otherwise
// the panel routine runs and the cursor is reset.
#include "cl_mouse.h"
extern "C" uint32_t d2c_94f10(uint32_t eax, uint32_t msg, const cl::Env& e) {
    if (clc::G(e, 0x11BC34) != 0xB) return eax;
    const uint32_t x = clm::X(e, msg), y = clm::Y(e, msg);
    clm::pos(e, x, y);
    if (!clm::in(e, 0x10B2F8, x, y)) {
        const bool in2 = (int32_t)x > (int32_t)clc::G(e, 0x10B28C) && (int32_t)x < (int32_t)clc::G(e, 0x10B290) &&
                         (int32_t)y >= (int32_t)clc::G(e, 0x10B294) && (int32_t)y <= (int32_t)clc::G(e, 0x10B298);
        if (!in2) {
            if (!e.H(0x8C550)) {
                e.H(0x8BBF0);
                return clm::reset(e);
            }
        }
    }
    return e.H(0x94AA0, msg);
}
