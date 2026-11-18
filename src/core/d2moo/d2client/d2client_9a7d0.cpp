// 1.13c D2Client+0x9A7D0 -- mouse move over the belt / inventory slot rects: needs the cursor object (callee 0x97CD0; EAX 0
// without). Clears the handled word at msg+0x18, then needs the mouse point (0x11B828, 0x11B824) inside the view
// (x in [0, width/2], y in [0, height - 0x31]). Keeps or drops the hover flag at 0x11BEF0 by a rect test around 0x11B9A0 /
// 0x11B9A4, and with a cursor item data word (+0x60) and no item on the cursor (callee 0xC202) walks the eleven slot rects
// at 0x11BD80 (stride 0x14): the slot under the point that differs from the hovered one (0x11BC3C) becomes the hover target
// and its centre goes to the selection words. EAX is modelled per exit.
#include "cl_mouse.h"
extern "C" uint32_t d2c_9a7d0(uint32_t, uint32_t msg, const cl::Env& e) {
    const uint32_t obj = e.H(0x97CD0);
    if (!obj) return obj;
    const int32_t px = (int32_t)clc::G(e, 0x11B828);
    e.W(msg + 0x18, 0);
    if (px < 0) return msg;
    int32_t half = (int32_t)clc::G(e, 0xDBC48);
    half = (half - (half >> 31)) >> 1;
    if (px > half) return (uint32_t)half;
    const int32_t py = (int32_t)clc::G(e, 0x11B824);
    if (py < 0) return (uint32_t)half;
    uint32_t eax = clc::G(e, 0xDBC4C);
    if (py > (int32_t)(eax - 0x31)) return eax;
    if (clc::G(e, 0x11BEF0)) {
        const uint32_t cy = clc::G(e, 0x11B9A4), ax = clc::G(e, 0x11B9A0);
        eax = e.H(0x8B4F0, cy + eax - 0x5f, ax + 0x130, cy + eax - 0x3f, 0, 0, 0, ax + 0x110);
        if (!eax) clc::S(e, 0x11BEF0, 0);
    }
    const uint32_t d = e.R(obj + 0x60);
    if (!d) return eax;
    e.H(0x8BBF0);
    eax = e.H(0xC202, e.R(clc::G(e, 0x11BBFC) + 0x60));
    if (eax) return eax;
    uint32_t idx = 0;
    for (uint32_t s = 0x11BD80; s < 0x11BE5C; s += 0x14, ++idx) {
        eax = clc::G(e, s);
        if ((int32_t)eax <= 0) continue;
        if (px < (int32_t)eax || px > (int32_t)clc::G(e, s + 4)) continue;
        eax = (uint32_t)py;
        if (py < (int32_t)clc::G(e, s + 8) || py > (int32_t)clc::G(e, s + 0xC)) continue;
        eax = e.H(0xC262, d, idx);
        if (!eax) continue;
        const uint32_t hit = eax;
        clc::S(e, 0x11BC2C, 1);
        if (hit == clc::G(e, 0x11BC3C)) continue;
        const int32_t sum = (int32_t)(clc::G(e, s) + clc::G(e, s + 4));
        const uint32_t mid = (uint32_t)((sum - (sum >> 31)) >> 1);
        clc::S(e, 0xE0EA8, mid);
        clc::S(e, 0xE0EB0, mid);
        clc::S(e, 0xE0EAC, clc::G(e, s + 8));
        eax = clc::G(e, s + 0xC);
        clc::S(e, 0xE0EB4, eax);
        clc::S(e, 0x11BC38, hit);
        clc::S(e, 0x11BC3C, hit);
    }
    return eax;
}
