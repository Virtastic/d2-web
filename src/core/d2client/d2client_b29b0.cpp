// 1.13c D2Client+0xB29B0 (ecx = unit, void): without a unit row run B1720. Otherwise, while the unit's counter
// (ordinal 10449) is below row[0x58] and a multiple of max(row[0x5c],1), move the unit by half the difference and
// half the sum of the two bounds (ordinals 10026 / 11002...), then tail B17C0.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB29B0
extern "C" void d2c_b29b0(uint32_t u, const cl::Env& e) {
    uint32_t row = u ? e.UnitRow(u) : 0;
    if (!row) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    int32_t d = (int32_t)e.R(row + 0x5c);
    if (d < 1) d = 1;
    int32_t n = (int32_t)e.H(0xc4d2, u);
    if (n < (int32_t)e.R(row + 0x58) && n % d == 0) {
        int32_t A = (int32_t)e.H(0xc322, u);
        int32_t B = (int32_t)e.H(0xc4ba, u);
        int32_t s1 = A - B, s2 = A + B;
        int32_t dx = (s1 + (s1 < 0)) >> 1, dy = (s2 + (s2 < 0)) >> 1;
        int32_t y = (int32_t)e.H(0x1660, 0, 0, 0, 0, 0, 0, u) + dy;
        int32_t x = (int32_t)e.H(0x1630, 0, 0, 0, 0, 0, 0, u) + dx;
        uint32_t o = e.R(u + 0x2c);
        e.H(0xc3ee, o, (uint32_t)x, (uint32_t)y);
        e.H(0xc376, e.R(u + 0x2c), u, 0);
        e.H(0xc448, u, (uint32_t)dx);
        e.H(0xc394, u, (uint32_t)dy);
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
