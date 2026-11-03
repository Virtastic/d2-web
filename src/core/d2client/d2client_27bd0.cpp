// 1.13c D2Client+0x27BD0: life orb fill. Every callee goes through the injected dispatcher.
#include "d2client_callee.h"
// 1.13c D2Client+0x27BD0
extern "C" void d2c_27bd0(const D2cSvc& v, uint32_t unit /*0x11BBFC*/, int32_t w /*0xDBC48 (unused)*/, int32_t h /*0xDBC4C*/, uint32_t cellA /*0x11A7CC*/, uint32_t cellB /*0x11A7E4*/) {
    (void)w;
    const int32_t cur = (int32_t)d2c_call(v, 0x26b20, 6, 0, 0, 0, 0);
    const int32_t mx = (int32_t)d2c_call(v, 0xc196, 0, 0, 0, 0, 0, unit);
    if (mx == 0) return;
    int32_t fill = (int32_t)((uint32_t)cur * 80u) / mx;
    const uint32_t* u = (const uint32_t*)(uintptr_t)unit;
    if (fill > 0 && fill <= 2 && unit && u[0] == 0 && u[4] != 0x11) fill = 2;   // unit+0x10
    uint32_t blk[18] = {};
    blk[13] = cellA; blk[0] = 0;
    const uint32_t bp = (uint32_t)(uintptr_t)blk;
    if (d2c_call(v, 0xc1fc, 0, 0, 0, 0, 0, unit, 0x64)) {
        int32_t total = (int32_t)((uint32_t)d2c_call(v, 0xc1c6, 0, 0, 0, 0, 0, unit, 0x4a, 0) * 80u) / 100;
        if (total > 0x50) total = 0x50;
        if (total > fill) {
            const uint32_t rd = d2c_call(v, 0xd10e, 0, 0, 0, 0, 0);
            d2c_call(v, 0xd1b6, 0, 0, 0, 0, 0, bp, rd == 6 ? 0x1cu : 0x1du, (uint32_t)(h - 13), (uint32_t)fill, (uint32_t)(total - fill), 0);
        }
    }
    blk[0] = d2c_call(v, 0xc1fc, 0, 0, 0, 0, 0, unit, 2) ? 2u : 0u;
    if (fill != 0) {
        d2c_call(v, 0xd10e, 0, 0, 0, 0, 0);
        d2c_call(v, 0xd1b6, 0, 0, 0, 0, 0, bp, 0x1d, (uint32_t)(h - 13), 0, (uint32_t)fill, 5);
    }
    blk[13] = cellB; blk[0] = 0;
    d2c_call(v, 0xd096, 0, 0, 0, 0, 0, bp, 0x1c, (uint32_t)(h - 5), 0xffffffffu, 5, 0);
}
