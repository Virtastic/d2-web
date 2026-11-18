// 1.13c D2Client+0x9ADB0 -- left-button press over the slot panel: needs the cursor object (callee 0x97CD0; EAX 0 without).
// Clears the handled word at msg+0x18; the mouse point (0x11B828, 0x11B824) must be inside the view (x in [0, width/2],
// y in [0, height - 0x31]), otherwise the handler returns with EAX = the failing coordinate bound. A point inside the
// panel rect (x in [0x11B9A0 + 0x110, + 0x130], y in [0x11B9A4 + h - 0x5F, + h - 0x3F]) raises the hover flag at
// 0x11BEF0, marks the message handled and calls the open routine (four zero words, EBX = 4); any other point only marks it handled.
#include "cl_mouse.h"
extern "C" uint32_t d2c_9adb0(uint32_t, uint32_t msg, const cl::Env& e) {
    uint32_t eax = e.H(0x97CD0);
    if (!eax) return eax;
    const int32_t px = (int32_t)clc::G(e, 0x11B828);
    e.W(msg + 0x18, 0);
    if (px < 0) return eax;
    int32_t half = (int32_t)clc::G(e, 0xDBC48);
    half = (half - (half >> 31)) >> 1;
    if (px > half) return (uint32_t)half;
    const int32_t py = (int32_t)clc::G(e, 0x11B824);
    if (py < 0) return (uint32_t)py;
    const uint32_t h = clc::G(e, 0xDBC4C);
    if (py > (int32_t)(h - 0x31)) return (uint32_t)py;
    const uint32_t ax = clc::G(e, 0x11B9A0), cy = clc::G(e, 0x11B9A4);
    if (px >= (int32_t)(ax + 0x110) && px <= (int32_t)(ax + 0x130) && py >= (int32_t)(cy + h - 0x5f) && py <= (int32_t)(cy + h - 0x3f)) {
        e.W(msg + 0x1C, 0);
        clc::S(e, 0x11BEF0, 1);
        e.W(msg + 0x18, 1);
        e.H(0x88A70, 0, 0, 0, 0, 0, 0, 0, 0, 4);
        return e.H(0xBE84, msg);
    }
    return clm::done(e, msg);
}
