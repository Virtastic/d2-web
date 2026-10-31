// 1.13c D2Client+0xB2EB0 (ecx = unit, void): after the partner / bounds checks (B1720 on a miss), and when the unit
// target (ordinal 11330) is valid, move the unit's object with C376 every row[0x58] (default 5) ticks while the
// separation (C190) lies in 4..24. Tail B17C0. 
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB2EB0
extern "C" void d2c_b2eb0(uint32_t u, const cl::Env& e) {
    uint32_t row = u ? e.UnitRow(u) : 0;
    if (!row) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    uint32_t bp = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    uint32_t t = e.H(0xc1f0, u);
    if (e.H(0xc208, t)) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    if (e.H(0xc322, u) & 1) {
        e.H(0x2a0b0, 0, 0, 0, 0, 0, u);
        uint32_t o = e.R(u + 0x2c);
        uint32_t d = e.H(0xc2da, o);
        if (d) {
            if (e.H(0xa5440, 0, 0, 0, 0, 0, d)) d = 0;
            else if (bp && !e.H(0xa62a0, bp, 0, 0, 0, 0, d)) d = 0;
        }
        int32_t k = (int32_t)e.R(row + 0x58);
        if (k <= 0) k = 5;
        if (d && (int32_t)e.H(0xc4d2, u) % k == 0) {
            int32_t s = (int32_t)e.H(0xc190, u, d);
            if (s > 3 && s < 0x19) e.H(0xc376, e.R(u + 0x2c), u, 0);
        }
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
