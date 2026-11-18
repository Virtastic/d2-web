// 1.13c D2Client+0xB25D0 (ecx = unit, void): every row[0x38] (min 1) ticks, seed the unit's roll state (+0x20) from
// C322, take C4BA's two signed words (x, y), roll one LCG step (lo' = lo*0x6AC690C5 + hi) and use its low bit to pick
// one of two quarter-turn mixes of (x, y); step the unit by that (each axis forced to a non-zero), move its object with
// C3EE / C376, then set C448 / C394 from C4AE and the packed pair. Missing row runs B1720; tail B17C0.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB25D0
extern "C" void d2c_b25d0(uint32_t u, const cl::Env& e) {
    uint32_t row = u ? e.UnitRow(u) : 0;
    if (!row) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    int32_t d = (int32_t)e.R(row + 0x38);
    if (d < 1) d = 1;
    if ((int32_t)e.H(0xc4d2, u) % d == 0) {
        uint32_t s = u + 0x20;
        uint32_t a = e.H(0xc322, u);
        e.H(0xc3b2, 0, 0, 0, 0, 0, 0, s, a);
        uint32_t b = e.H(0xc4ba, u);
        int32_t x = (int16_t)(b & 0xffff), y = (int16_t)(b >> 16);
        uint64_t p = (uint64_t)e.R(s) * 0x6AC690C5ull + e.R(s + 4);
        e.W(s, (uint32_t)p);
        e.W(s + 4, (uint32_t)(p >> 32));
        int32_t u1, u2;
        if ((uint32_t)p & 1) { u1 = -y + 4 * x; u2 = x + 4 * y; }
        else { u1 = y + 4 * x; u2 = -x + 4 * y; }
        int32_t dx = (u1 + (u1 < 0 ? 3 : 0)) >> 2, dy = (u2 + (u2 < 0 ? 3 : 0)) >> 2;
        if (!dx) dx = 1;
        if (!dy) dy = 1;
        int32_t py = (int32_t)e.H(0x1660, 0, 0, 0, 0, 0, 0, u) + dy;
        int32_t px = (int32_t)e.H(0x1630, 0, 0, 0, 0, 0, 0, u) + dx;
        e.H(0xc3ee, e.R(u + 0x2c), (uint32_t)px, (uint32_t)py);
        e.H(0xc376, e.R(u + 0x2c), u, 0);
        uint32_t k = e.H(0xc4ae, 0, 0, 0, 0, 0, 0, s);
        e.H(0xc448, u, k);
        e.H(0xc394, u, ((uint32_t)dy & 0xffff) << 16 | ((uint32_t)dx & 0xffff));
        (void)0;
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
