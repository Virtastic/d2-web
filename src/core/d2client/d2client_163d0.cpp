// 1.13c D2Client+0x163D0 -- mouse message handler (stdcall, msg): stores the cursor x/y words of msg+0xC/+0xE at 0x6FBCB828/0x6FBCB824.
// With the pointer state calls 0xD1BC and 0xC0B2 saying "clamp" (0xD1BC nonzero, 0xC0B2 zero) the position is clamped to the screen
// size (signed, to size-1) and, when either coordinate was clamped, written back with SetCursorPos and the handler returns. Otherwise
// 0xD1C2(1), the tick stamp 0x6FBCB860 and the cursor-state steps (states 4 and 2 become 3).
#include "d2client_sn18.h"
extern "C" uint32_t d2c_163d0(uint32_t msg, const cl::Env& e) {
    e.W(0x6fbcb828, sn18::R16(e, msg + 0xc));
    e.W(0x6fbcb824, sn18::R16(e, msg + 0xe));
    if (e.H(0xD1BC) && !e.H(0xC0B2)) {
        int32_t x = (int32_t)e.R(0x6fbcb828), y;
        const int32_t mx = (int32_t)e.R(0x6fb8bc48) - 1;
        bool cx = false;
        if (x > mx) { x = mx; e.W(0x6fbcb828, (uint32_t)x); cx = true; }
        y = (int32_t)e.R(0x6fbcb824);
        const int32_t my = (int32_t)e.R(0x6fb8bc4c) - 1;
        bool cy = false;
        if (y > my) { y = my; e.W(0x6fbcb824, (uint32_t)y); cy = true; }
        if (cx || cy) {
            const uint32_t r = e.H(0xCF0B4, (uint32_t)x, (uint32_t)y);
            e.W(0x6fbcb828, (uint32_t)x);
            e.W(0x6fbcb824, (uint32_t)y);
            return r;
        }
    }
    e.H(0xD1C2, 1);
    e.W(0x6fbcb860, e.H(sn18::kTick));
    const uint32_t st = e.R(0x6fbcb864);
    uint32_t ret = st;
    if (st == 4 || st == 2) {
        e.W(0x6fbcb864, 3);
        e.W(0x6fbcb854, 2);
        e.W(0x6fbcb858, 0);
        if (e.R(0x6fba861c) != 7) {
            const uint32_t s = e.H(0xBF92, 0x9a);
            e.H(0xBF80, 0x6fb8359f, s);
            ret = e.H(0x3664, 0xffffffffu);
        }
        e.W(0x6fbcb858, (e.R(0x6fba8598) << 8) - 0x100u);
    }
    e.W(msg + 0x18, 0);
    e.W(msg + 0x1c, 0);
    return ret;
}
