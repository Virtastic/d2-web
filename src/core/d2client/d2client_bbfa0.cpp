// 1.13c D2Client+0xBBFA0 (ecx = unit): missile row with a non-negative +0x2C word and a room: count = (level-1)*row+0x70 + row+0x6C (level from 0xC3FA); when not positive it is looked up from the skill row of the 0xC388 id (0x15B0, +0x144 through 0xC316), minimum 1. Calls 0xA27B0 with the count and a quarter of it; with a non-negative +0x2E word calls 0xB0450 with the unit's 0xC3FA/0xC388/0x1660/0x1630 values. Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_bbfa0(uint32_t u, const cl::Env& e) {
    if (!u) return 1;
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x2c) < 0) return 1;
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!room) return 1;
    uint32_t lvl = e.H(0xc3fa, u);
    int32_t n = (int32_t)((lvl - 1) * e.R(row + 0x70) + e.R(row + 0x6c));
    if (n <= 0) {
        uint32_t sid = e.H(0xc388, u);
        uint32_t sk = e.H(0x15b0, 0, 0, 0, 0, 0, sid);
        if (!sk) return 1;
        n = (int32_t)e.H(0xc316, room, e.R(sk + 0x144), sid, lvl);
        if (n < 1) n = 1;
    }
    uint32_t j = e.H(0xc388, u);
    e.Xs(lvl);   // sixth stack word
    e.H(0xa27b0, u, (uint32_t)e.R16s(row + 0x2c), (uint32_t)n, (uint32_t)((n + ((n >> 31) & 3)) >> 2), j);
    if (e.R16s(row + 0x2e) >= 0) {
        uint32_t k = e.H(0xc3fa, u);
        uint32_t g = e.H(0xc388, u);
        uint32_t w = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
        uint32_t v = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
        e.H(0xb0450, (uint32_t)e.R16s(row + 0x2e), v, w, g, k);
    }
    return 1;
}
