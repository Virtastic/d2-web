// 1.13c D2Client+0x26CD0: free the 17 HUD cell globals (D2CMP #10065 then Fog #10043 on each non-null) and clear the 12-dword block at 0x11A794.
#include "d2client_callee.h"
// 1.13c D2Client+0x26CD0
extern "C" void d2c_26cd0(const D2cSvc& v, uint32_t* g /*30 dwords at D2Client+0x11A770*/) {
    static const uint16_t order[17] = { 0x7e0, 0x7c8, 0x7dc, 0x788, 0x7c4, 0x778, 0x774, 0x7d8, 0x77c, 0x780, 0x7cc, 0x7e4, 0x7d4, 0x790, 0x784, 0x770, 0x78c };
    for (int i = 0; i < 17; ++i) {
        uint32_t& c = g[(order[i] - 0x770) >> 2];
        if (c) {
            d2c_call(v, 0xc18a, 0, 0, 0, 0, 0, c);                     // D2CMP #10065 (cell)
            d2c_call(v, 0xbf86, 0, c, 0xd6ab0, 0, 0, 0x7a, 0);         // Fog #10043 free (ECX = cell, EDX = D2Client+0xD6AB0 file name, 0x7a line, 0)
        }
        c = 0;
    }
    for (int i = 9; i < 21; ++i) g[i] = 0;                            // 0x11A794..0x11A7C3
}
