// 1.13c D2Client+0x27A90: mana orb fill. Every callee goes through the injected dispatcher.
#include "d2client_callee.h"
// 1.13c D2Client+0x27A90
extern "C" void d2c_27a90(const D2cSvc& v, uint32_t unit /*0x11BBFC*/, int32_t w /*0xDBC48*/, int32_t h /*0xDBC4C*/, uint32_t cellA /*0x11A7CC*/, uint32_t cellB /*0x11A7E4*/) {
    int32_t cur = (int32_t)d2c_call(v, 0x26b20, 8, 0, 0, 0, 0);
    const int32_t mx = (int32_t)d2c_call(v, 0xcf4c, 0, 0, 0, 0, 0, unit);
    if (mx == 0) return;
    if (cur > mx) cur = mx;
    const int32_t fill = (int32_t)((uint32_t)cur * 80u) / mx;
    uint32_t blk[18] = {};
    blk[13] = cellA; blk[0] = 1;
    const uint32_t bp = (uint32_t)(uintptr_t)blk;
    if (d2c_call(v, 0xc1fc, 0, 0, 0, 0, 0, unit, 0x6a)) {
        int32_t total = (int32_t)((uint32_t)d2c_call(v, 0xc1c6, 0, 0, 0, 0, 0, unit, 0x1a, 0) * 80u) / 100;
        if (total > 0x50) total = 0x50;
        if (total > fill) {
            const uint32_t rd = d2c_call(v, 0xd10e, 0, 0, 0, 0, 0);
            d2c_call(v, 0xd1b6, 0, 0, 0, 0, 0, bp, (uint32_t)(w + (rd == 6 ? -0x70 : -0x6f)), (uint32_t)(h - 13), (uint32_t)fill, (uint32_t)(total - fill), 0);
        }
    }
    d2c_call(v, 0xd10e, 0, 0, 0, 0, 0);
    d2c_call(v, 0xd1b6, 0, 0, 0, 0, 0, bp, (uint32_t)(w - 0x6f), (uint32_t)(h - 13), 0, (uint32_t)fill, 5);
    blk[13] = cellB; blk[0] = 1;
    d2c_call(v, 0xd096, 0, 0, 0, 0, 0, bp, (uint32_t)(w - 0x6e), (uint32_t)(h - 9), 0xffffffffu, 5, 0);
}
