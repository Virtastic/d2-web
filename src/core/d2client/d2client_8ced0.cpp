// 1.13c D2Client+0x8CED0 -- a click on the panel's close box (ESI = message; returns 1 when consumed): y = msg word +0xE, x = word
// +0xC; msg+0x18 = 1, +0x1C = 0, Storm 511(msg). Inside x in [W - dx - 0x12E, W - dx - 0x10E] and y in [dy + H - 0x60, dy + H - 0x40]
// ([0xDBC48] W, [0xDBC4C] H, [0x11B9A0] dx, [0x11B9A4] dy): [0x11BEDC] = 1, the message marked again and handed to Storm 511 once
// more. g = pinned globals (+0 dx, +4 dy, +8 W, +0xC H, +0x10 the close flag).
#include "cl_env.h"
extern "C" uint32_t d2c_8ced0(uint32_t msg, const cl::Env& e, uint32_t g) {
    int32_t y = (int32_t)(e.R(msg + 0xc) >> 16), x = (int32_t)(e.R(msg + 0xc) & 0xffff);
    e.W(msg + 0x18, 1); e.W(msg + 0x1c, 0);
    e.H(0xbe84, msg);
    int32_t r = (int32_t)e.R(g + 8) - (int32_t)e.R(g);
    if (x < r - 0x12e || x > r - 0x10e) return 0;
    int32_t b = (int32_t)e.R(g + 4) + (int32_t)e.R(g + 0xc);
    if (y < b - 0x60 || y > b - 0x40) return 0;
    e.W(g + 0x10, 1);
    e.W(msg + 0x18, 1); e.W(msg + 0x1c, 0);
    e.H(0xbe84, msg);
    return 1;
}
