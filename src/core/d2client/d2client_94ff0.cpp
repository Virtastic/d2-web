// 1.13c D2Client+0x94FF0 -- left-button release in the game views (screen states 0..4; EAX unchanged otherwise): stores the
// mouse position. With the cursor item set (0x11B834) and the point inside the cursor rect (0x10B408..) the handler returns
// x. Otherwise, in a non-zero state with a pending object (0x119721), it finds that object in the room-object hash
// (0x10A808, link +0xE4, id +0xC), asserts it is type 1 (the failure path reports and exits), re-tests the point against the
// cursor rect and the drop rect (0x10B3B4..), and calls the drop routine with the object's data (+0x60). Every other path
// hands the message to the move body.
#include "cl_mouse.h"
extern "C" uint32_t d2c_94ff0(uint32_t eax, uint32_t msg, const cl::Env& e) {
    const uint32_t st = clc::G(e, 0x11BC34);
    if (st > 4) return eax;
    const bool cur = clc::G(e, 0x11B834) != 0;
    const uint32_t lo = clc::G(e, 0x10B408);
    uint32_t x = clm::X(e, msg);
    const uint32_t y = clm::Y(e, msg);
    clm::pos(e, x, y);
    if (cur && (int32_t)x >= (int32_t)lo && (int32_t)x <= (int32_t)clc::G(e, 0x10B40C) &&
        (int32_t)y >= (int32_t)clc::G(e, 0x10B410) && (int32_t)y <= (int32_t)clc::G(e, 0x10B414))
        return x;
    if (st && clc::G(e, 0x119721)) {
        const uint32_t id = clc::G(e, 0x11971D);
        uint32_t n = clc::G(e, 0x10A808 + (id & 0x7F) * 4);
        while (n && e.R(n + 0xC) != id) n = e.R(n + 0xE4);
        if (n) {
            if (e.R(n) != 1) {
                const uint32_t r = e.H(0xBF92, 0x5A);
                e.H(0xBF80, 0x6FB8359Fu, r, 0x5A);
                x = e.H(0x3664, 0xFFFFFFFFu);
            }
            const uint32_t d = e.R(n + 0x60);
            if ((int32_t)x >= (int32_t)lo && (int32_t)x <= (int32_t)clc::G(e, 0x10B40C) &&
                (int32_t)y >= (int32_t)clc::G(e, 0x10B410) && (int32_t)y <= (int32_t)clc::G(e, 0x10B414)) {
                e.W(msg + 0x18, 1);
                e.W(msg + 0x1C, 0);
                if ((int32_t)x >= (int32_t)clc::G(e, 0x10B3B4) && (int32_t)x <= (int32_t)clc::G(e, 0x10B3B8) &&
                    (int32_t)y >= (int32_t)clc::G(e, 0x10B3BC) && (int32_t)y <= (int32_t)clc::G(e, 0x10B3C0)) {
                    eax = e.H(0x945D0, d, 0, x, y, clc::G(e, 0x11BC48) & 0xFF, 0x6FBBB3B0u);
                    e.W(msg + 0x1C, 0);
                    e.W(msg + 0x18, 1);
                    clc::S(e, 0x0E0EA4, 0);
                    return eax;
                }
            }
        }
    }
    return e.H(0x94AA0, msg);
}
