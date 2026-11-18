// 1.13c D2Client+0xb72d0 -- client missile: a mirrored pair of children (offset (a, b) and (-a, -b)). ECX = unit, no return.
// No unit / class / row, class word row +0x1E < 0, or no room (0xA60D0): 0xB1720() and done. D2Common 0xC352 on the
// unit path (+0x2C) == 0 skips to the tail. a = 0xC322(u), b = 0xC4BA(u); request type 2, room, unit, class row +0x1E,
// words 7/8 = (a, b), then the same block with (-a, -b). Tail: 0xB17C0(ECX = u).
#include "cl_env.h"
extern "C" void d2c_b72d0(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    uint32_t room = 0;
    if (!row || e.R16s(row + 0x1e) < 0 || !(room = e.H(0xa60d0, 0, 0, 0, 0, 0, u))) { e.H(0xb1720); return; }
    if (e.H(0xc352, e.R(u + 0x2c))) {
        uint32_t a = e.H(0xc322, u);
        uint32_t b = e.H(0xc4ba, u);
        uint32_t q[23] = {};
        q[0] = 2; q[1] = room; q[2] = u;
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        q[4] = (uint32_t)e.R16s(row + 0x1e);
        q[7] = a; q[8] = b;
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        q[7] = 0u - a; q[8] = 0u - b;
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
