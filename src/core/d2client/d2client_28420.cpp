// 1.13c D2Client+0x28420: durability warning cel; frame = severity*11 + slot offset, drawn at (W-0x3c, y).
#include "d2client_callee.h"
// 1.13c D2Client+0x28420
extern "C" void d2c_28420(const D2cSvc& v, uint32_t sev, uint32_t special, uint32_t flag, uint32_t slot, int32_t w, uint32_t cell /*0x11A7C4*/, uint32_t expFlag /*0xFADA8*/) {
    uint32_t off = 0;
    switch (slot) {
        case 1: off = 1; break;
        case 3: off = 4; break;
        case 4: off = special ? 10 : (flag ? 7 : 6); break;
        case 5: off = special ? 9 : (flag ? 5 : 8); break;
        case 8: off = 2; break;
        case 9: off = 3; break;
        default: break;
    }
    uint32_t blk[18] = {}; blk[0] = sev * 11 + off; blk[13] = cell;
    uint32_t y = 0x6e;
    if (expFlag) y = 0x8c + (d2c_call(v, 0x9db90, 0, 0, 0, 0, 0) ? 0x2b : 0);
    d2c_call(v, 0xd096, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)blk, (uint32_t)(w - 0x3c), y, 0xffffffffu, 1, 0);   // D2Gfx #10041
}
