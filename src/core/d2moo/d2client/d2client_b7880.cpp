// 1.13c D2Client+0xb7880 -- client missile: path-bound mirrored pair. ECX = unit, no return.
// No unit: 0xA6EF0(ECX = the client unit-hash table at D2Client+0x109A08 + 6 * 0x200 (unrecorded by the vector), EDX = 0x7F; push 6, -1).
// No class / row, class word row +0x1E < 0, or no path (+0x2C): the same with the unit's type / guid (EDX = guid & 0x7F).
// Otherwise x/y = 0x1630 / 0x1660(ECX = u); t = D2Common 0xC4D2(u) < 2 sets 0xC394(u, 0xC31C(path) - x) and
// 0xC448(u, y - 0xC340(path)). When 0xC352(path) != 0: request 0xB, room (0xA60D0), at (x, y), class row +0x1E,
// word 13 = row +0x58, words 7/8 = (0xC322(u), 0xC4BA(u)), then again with both negated (re-read). Tail 0xB17C0(ECX = u).
#include "cl_env.h"
extern "C" void d2c_b7880(uint32_t u, const cl::Env& e) {
    uint32_t ty = 6, gd = 0xffffffffu, row = 0;
    if (u) {
        row = e.UnitRow(u);
        if (!row || e.R16s(row + 0x1e) < 0 || !e.R(u + 0x2c)) { ty = e.R(u); gd = e.R(u + 0xc); row = 0; }
    }
    if (!row) { e.H(0xa6ef0, gd, ty, 0, 0, 0, 0, 0, gd & 0x7f); return; }
    uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
    uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
    if ((int32_t)e.H(0xc4d2, u) < 2) {
        uint32_t a = e.H(0xc31c, e.R(u + 0x2c)) - x;
        uint32_t b = y - e.H(0xc340, e.R(u + 0x2c));
        e.H(0xc448, u, b);
        e.H(0xc394, u, a);
    }
    if (e.H(0xc352, e.R(u + 0x2c))) {
        uint32_t q[23] = {};
        q[0] = 0xb;
        q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
        q[5] = x; q[6] = y;
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        q[4] = (uint32_t)e.R16s(row + 0x1e);
        q[13] = e.R(row + 0x58);
        q[7] = e.H(0xc322, u);
        q[8] = e.H(0xc4ba, u);
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        q[7] = 0u - e.H(0xc322, u);
        q[8] = 0u - e.H(0xc4ba, u);
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
