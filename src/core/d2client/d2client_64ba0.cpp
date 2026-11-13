// 1.13c D2Client+0x64BA0 -- left-click on the row of small buttons under the mini panel: when the click lies inside the strip, find the button under it and
// raise its flag (the first ones stay clear while the level bar blocks them), then queue the click message.
#include "cl_cmd.h"
extern "C" uint32_t d2c_64ba0(uint32_t msg, const cl::Env& e) {
    using namespace clc;
    const uint32_t cnt = G(e, 0x11C098) ? 7 : 8;
    const uint32_t pl = G(e, kPlayer);
    if (!pl) return 0;
    const uint32_t inv = e.R(pl + 0x60);
    if (!inv) return 0;
    uint32_t eax = e.H(0xc202, inv);
    if (eax) return eax;
    eax = G(e, 0x11C090);
    if (eax) return eax;
    if (!G(e, kPlayer)) return 0;
    const int32_t x = (int32_t)(e.R(msg + 0xC) & 0xffffu), y = (int32_t)(e.R(msg + 0xE) & 0xffffu);
    int32_t off = 0;
    if (G(e, 0x11C474) && (int32_t)e.H(0x26500) > 1) off = -0x76;
    if (G(e, 0x11C088)) off -= 0x76;
    if (G(e, 0x11C08C)) off += 0x77;
    const int32_t w = (int32_t)G(e, 0xDBC48);
    const int32_t c = ((w - (w >> 31)) >> 1) + off;
    if (x <= c - 0x54) return (uint32_t)c;
    if (x >= c + 0x59) return (uint32_t)(c + 0x59);
    const int32_t h = (int32_t)G(e, 0xDBC4C);
    if (y <= h - 0x45) return (uint32_t)h;
    if (y >= h - 0x2F) return (uint32_t)(h - 0x2F);
    uint32_t i = 0;
    for (;; ++i) {
        if (i >= cnt) return cnt;
        const int32_t l = (int32_t)G(e, 0x1154F0 + 4 * i);
        if (x > l && x < l + 0x14) break;
    }
    bool set = true;
    if (G(e, 0x115530 + 4 * i) != 7) {
        const uint32_t p = G(e, kPlayer);
        if (e.R(p) != 0 || e.R(p + 0x10) == kDeadMode) set = false;
        else if (G(e, 0xFADA4) && (int32_t)i < (G(e, 0x11C394) ? 5 : 4)) set = false;
    }
    if (set) S(e, 0x115550 + 4 * i, 1);
    e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 4);
    S(e, 0x11C09C, 1);
    e.W(msg + 0x18, 1);
    e.W(msg + 0x1C, 0);
    return e.H(0xbe84, msg);
}
