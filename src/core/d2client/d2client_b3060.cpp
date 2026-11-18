// 1.13c D2Client+0xB3060 (ecx = unit, void): when ordinal 11372 is non-zero set bit 16 of the unit's flags and
// call ordinal 11000 on its object (0, the data-section tag, 0x7f). Then, when the unit lies inside the view window
// (ordinals 10..: C1D2 / C1F6 against the screen origin and extents g[0..4]), set C2C8(unit, 0x80) and tail B17C0;
// outside it call B1420(unit, 0). g = {0x119960, 0x11C418, 0xF9E14, 0x11995C, 0xF9E18} (module globals).
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB3060
extern "C" void d2c_b3060(uint32_t u, uint32_t g, const cl::Env& e) {
    if (e.H(0xc46c, u)) {
        if (u) e.W(u + 0xc4, e.R(u + 0xc4) | 0x10000);
        e.H(0xc328, e.R(u + 0x2c), 0, 0xB20D4010, 0x7f);
    }
    uint32_t edi = e.R(g + 0) - e.R(g + 4);
    int32_t x = (int32_t)(e.H(0xc1d2, u) - edi);
    if (x >= 0 && x <= (int32_t)e.R(g + 8)) {
        uint32_t edi2 = e.R(g + 12) - 8;
        int32_t y = (int32_t)(e.H(0xc1f6, u) - edi2);
        if (y >= 0 && y <= (int32_t)(e.R(g + 16) + 0x40)) {
            e.H(0xc2c8, u, 0x80);
            e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
            return;
        }
    }
    e.H(0xb1420, 0, 0, 0, 0, 0, 0, u);
}
