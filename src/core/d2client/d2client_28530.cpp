// 1.13c D2Client+0x28530: warning icon cel; takes the first free of six 40px slots (flags at 0x11C488).
#include "d2client_callee.h"
// 1.13c D2Client+0x28530
extern "C" void d2c_28530(const D2cSvc& v, uint32_t frame, int32_t w, uint32_t cell /*0x11A788*/, uint32_t expFlag /*0xFADA8*/, uint8_t* slots /*0x11C488, 6 bytes*/) {
    int32_t x = w - 0x37;
    uint32_t y = 0x9b;
    for (uint32_t i = 0; i < 6; ++i) if (!slots[i]) { slots[i] = 1; x += (int32_t)i * -40; break; }
    uint32_t blk[18] = {}; blk[0] = frame; blk[13] = cell;
    if (expFlag) { if (d2c_call(v, 0x9db90, 0, 0, 0, 0, 0)) y = 0xc6; y += 0x1e; }
    d2c_call(v, 0xd096, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)blk, (uint32_t)x, y, 0xffffffffu, 5, 0);   // D2Gfx #10041
}
