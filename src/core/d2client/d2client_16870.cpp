// 1.13c D2Client+0x16870 -- cursor draw (cdecl, void): the cursor position 0x6FBCB828/0x6FBCB824 and width 0x6FBCB82C are clamped to the
// screen (size 0x6FB8BC48 x 0x6FB8BC4C) and the 18-dword cel block (frame at +0, cel at +0x34) is drawn through 0xD096, then 0x167D0.
#include <cstring>
#include "d2client_sn18.h"
extern "C" uint32_t d2c_16870(const cl::Env& e) {
    const int32_t x = (int32_t)e.R(0x6fbcb828), w = (int32_t)e.R(0x6fbcb82c), y = (int32_t)e.R(0x6fbcb824);
    int32_t cx = x + w, cy;
    if (y < 0) cy = 0;
    else { cy = y; const int32_t m = (int32_t)e.R(0x6fb8bc4c) - 1; if (y > m) cy = m; }
    if (cx >= w) { const int32_t m = (int32_t)e.R(0x6fb8bc48) - w - 1; if (cx > m) cx = m; }
    else cx = w;
    uint32_t b[18];
    memset(b, 0, sizeof b);
    b[13] = e.R(0x6fbcb838 + e.R(0x6fbcb854) * 4);
    b[0] = (uint32_t)((int32_t)e.R(0x6fbcb858) >> 8);
    const uint32_t bp = (uint32_t)(uintptr_t)b;
    e.H(0xD096, bp, (uint32_t)cx, (uint32_t)cy, 0xffffffffu, 5);
    sn18::blk(e, 0xD096, bp, 18);
    return e.H(0x167D0);
}
