// 1.13c D2Client+0x94DF0 -- mouse move in screen state 0xE: stores the position, hands a move inside either of the two
// panel rects (0x10B2F8.., 0x10B3CC..) to the move body, otherwise clears the hover words and resets the cursor.
#include "cl_mouse.h"
extern "C" uint32_t d2c_94df0(uint32_t eax, uint32_t msg, const cl::Env& e) {
    if (clc::G(e, 0x11BC34) != 0xE) return eax;
    const uint32_t x = clm::X(e, msg), y = clm::Y(e, msg);
    clm::pos(e, x, y);
    if (clm::in(e, 0x10B2F8, x, y) || clm::in(e, 0x10B3CC, x, y)) return e.H(0x94AA0, msg);
    clm::hov(e);
    return clm::reset(e);
}
