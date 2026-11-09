// 1.13c D2Client+0xb8130 -- client missile: diagonal drift with a trail child. ECX = unit, no return.
// No unit / bad class: 0xB1720(ESI = unit) (row +0x1E is not checked here). owner = 0xA60D0(EAX = unit); none: the
// 0xB17C0 tail. When row +0x1E > 0 and 0xC352(path) != 0: block word 1 = owner, 2 = unit, 11 = 0xC388(unit),
// 12 = 0xC3FA(unit), 4 = row +0x1E, 0xB30F0(ECX = block). When 0xC4D2(unit) % max(row +0x58, 1) == 0: a = 0xC322(unit),
// b = 0xC4BA(unit), sel = LCG(unit seed) & 3: sel 0 turns (a,b) by -45 degrees at 3/4 length, sel 2 by +45, otherwise
// as is; 0xC3EE(path, 0x1630(unit) + dx, 0x1660(unit) + dy) (0x1660 first), 0xC376(path, unit, 0), 0xC448(unit, dx),
// 0xC394(unit, dy). Always the 0xB17C0(ECX = unit) tail.
#include "cl_env.h"
namespace { inline uint32_t q34(uint32_t x) { return (uint32_t)((int32_t)(x * 3u) / 4); } }
extern "C" void d2c_b8130(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    uint32_t own = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!own) { e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u); return; }
    if (e.R16s(row + 0x1e) > 0 && e.H(0xc352, e.R(u + 0x2c))) {
        uint32_t q[23] = {};
        q[1] = own; q[2] = u;
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        q[4] = (uint32_t)e.R16s(row + 0x1e);
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    }
    int32_t n = (int32_t)e.R(row + 0x58); if (n < 1) n = 1;
    if ((int32_t)e.H(0xc4d2, u) % n == 0) {
        uint32_t a = e.H(0xc322, u);
        uint32_t b = e.H(0xc4ba, u);
        uint32_t sel = e.Lcg(u + 0x20) & 3;
        uint32_t dx = a, dy = b;
        if (sel == 0) { dx = q34(a - b); dy = q34(b + a); }
        else if (sel == 2) { dx = q34(b + a); dy = q34(b - a); }
        uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, u) + dy;
        uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, u) + dx;
        e.H(0xc3ee, e.R(u + 0x2c), x, y);
        e.H(0xc376, e.R(u + 0x2c), u, 0);
        e.H(0xc448, u, dx);
        e.H(0xc394, u, dy);
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
