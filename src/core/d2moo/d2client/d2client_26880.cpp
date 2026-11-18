// 1.13c D2Client+0x26880: belt-toggle button, cell draw. Frame 0/1 collapsed (hot), 2/3 expanded.
#include "d2client_callee.h"
// 1.13c D2Client+0x26880
extern "C" void d2c_26880(const D2cSvc& v, int32_t w, int32_t h, int32_t mouseA /*0x11B828*/, int32_t mouseB /*0x11B824*/,
                          uint32_t expanded /*0xFADD4*/, uint32_t hot /*0x11C4A4*/, uint32_t cell /*0x11A7D4*/) {
    uint32_t blk[18] = {};
    blk[13] = cell;
    const int32_t w2 = (w - (w >> 31)) >> 1;
    const bool in = mouseA >= w2 - 8 && mouseA <= w2 + 5 && mouseB >= h - 0x27 && mouseB <= h - 0xd;
    blk[0] = expanded ? (hot && in ? 3u : 2u) : (hot && in ? 1u : 0u);
    d2c_call(v, 0xd096, 0, h, 0, 0, w, (uint32_t)(uintptr_t)blk, (uint32_t)(w2 - 8), (uint32_t)(h - 0x10), 0xffffffffu, 5, 0);   // D2Gfx #10041 cel draw
}
