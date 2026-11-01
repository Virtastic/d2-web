// 1.13c D2Client+0x28320: bottom-centre button: cel frame from two flags and the 0x265A0 hover test, then the hint (0x27EF0) when the mouse is over it.
#include "d2client_callee.h"
// 1.13c D2Client+0x28320
extern "C" void d2c_28320(const D2cSvc& v, int32_t w, int32_t h, int32_t mx /*0x11B828*/, int32_t my /*0x11B824*/, uint32_t flagA /*0x11C3EC*/, uint32_t flagB /*0x11C4AC*/, uint32_t cell /*0x11A770*/) {
    uint32_t blk[18] = {}; blk[13] = cell;
    if (flagA) blk[0] = (flagB && d2c_call(v, 0x265a0, 0, (uint32_t)mx, 0, 0, 0)) ? 3 : 2;
    else blk[0] = (flagB && d2c_call(v, 0x265a0, 0, (uint32_t)mx, 0, 0, 0)) ? 1 : 0;
    const int32_t w2 = (w - (w >> 31)) >> 1;
    d2c_call(v, 0xd096, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)blk, (uint32_t)(w2 - 0x91), (uint32_t)(h - 10), 0xffffffffu, 5, 0);   // D2Gfx #10041
    if (mx >= w2 - 0x91 && mx <= w2 - 0x80 && my >= h - 0x1c && my <= h - 8) d2c_call(v, 0x27ef0, 0, 0, 0, 0, 0);
}
