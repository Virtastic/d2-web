// 1.13c D2Client+0xB2A80 (ecx = unit, void): step the unit toward its A60D0 partner's target by row[0x68] (min 1) per
// axis, every row[0x60] (default 5) ticks while the separation (ordinal 49552) is within row[0x64]. Missing row or
// partner runs B1720; otherwise tail B17C0. The empty-[unit+0x2c] fatal-assert path is not modelled (no vector reaches it).
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB2A80
extern "C" void d2c_b2a80(uint32_t u, const cl::Env& e) {
    uint32_t row = u ? e.UnitRow(u) : 0;
    uint32_t p = row ? e.H(0xa60d0, 0, 0, 0, 0, 0, u) : 0;
    if (!row || !p) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    e.H(0x2a0b0, 0, 0, 0, 0, 0, u);
    uint32_t o = e.R(u + 0x2c);
    if (!o) return;
    uint32_t b = e.H(0xc2da, o);
    if (!b) b = e.H(0xa5f70);
    if (b) {
        int32_t k = (int32_t)e.R(row + 0x60);
        if (k <= 0) k = 5;
        int32_t n = (int32_t)e.H(0xc4d2, u);
        if (n % k == 0 && (int32_t)e.H(0xc190, u, b) <= (int32_t)e.R(row + 0x64)) {
            int32_t sx = (int32_t)e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
            int32_t sy = (int32_t)e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
            int32_t tx = (int32_t)e.H(0x1630, 0, 0, 0, 0, 0, 0, b);
            int32_t ty = (int32_t)e.H(0x1660, 0, 0, 0, 0, 0, 0, b);
            int32_t st = (int32_t)e.R(row + 0x68);
            if (st < 1) st = 1;
            int32_t dx = tx > sx ? st : tx < sx ? -st : 0;
            int32_t dy = ty > sy ? st : ty < sy ? -st : 0;
            int32_t y = (int32_t)e.H(0x1660, 0, 0, 0, 0, 0, 0, u) + dy;
            int32_t x = (int32_t)e.H(0x1630, 0, 0, 0, 0, 0, 0, u) + dx;
            e.H(0xc3ee, e.R(u + 0x2c), (uint32_t)x, (uint32_t)y);
            e.H(0xc376, e.R(u + 0x2c), u, 0);
        }
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
