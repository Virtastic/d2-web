// 1.13c D2Client+0xb8550 -- client missile: diagonal steps with a trailing child. ECX = unit, no return.
// No unit / bad class: 0xB1720(ESI = unit). owner = 0xA60D0(EAX = unit); none: the 0xB17C0 tail. When row +0x1E >= 0 and
// 0xC352(path) != 0: block word 1 = owner, 2 = unit, 11 = 0xC388(unit), 12 = 0xC3FA(unit), 4 = row +0x1E, 21 = 0xB0F40,
// 22 = unit; m = 0xB30F0(ECX = block); m: 0xC7E30(EAX = m, ECX = 0xC7EC0(EAX = unit), EDX = 0, 0).
// When 0xC4D2(unit) % max(row +0x58, 1) == 0: a = 0xC322(unit), b = 0xC4BA(unit), LCG(unit seed) & 1 picks
// (a-b, b+a) or (b+a, b-a), each times 3/4 (truncating); a non-zero step moves the path to 0x1630/0x1660(unit) + step
// (0xC3EE, 0x1660 first; then 0xC376(path, unit, 0)); always 0xC448(unit, dx), 0xC394(unit, dy). Then the 0xB17C0 tail.
#include "cl_env.h"
namespace { inline uint32_t q34(uint32_t x) { return (uint32_t)((int32_t)(x * 3u) / 4); } }
extern "C" void d2c_b8550(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    uint32_t own = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!own) { e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u); return; }
    if (e.R16s(row + 0x1e) >= 0 && e.H(0xc352, e.R(u + 0x2c))) {
        uint32_t q[23] = {};
        q[1] = own; q[2] = u;
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        q[4] = (uint32_t)e.R16s(row + 0x1e);
        q[21] = 0xB2000000u | 0xb0f40u; q[22] = u;
        uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        if (m) {
            uint32_t c = e.H(0xc7ec0, 0, 0, 0, 0, 0, u);
            e.H(0xc7e30, 0, 0, 0, 0, 0, m, c, 0);
        }
    }
    int32_t n = (int32_t)e.R(row + 0x58); if (n < 1) n = 1;
    if ((int32_t)e.H(0xc4d2, u) % n == 0) {
        uint32_t a = e.H(0xc322, u);
        uint32_t b = e.H(0xc4ba, u);
        uint32_t X, Y;
        if (e.Lcg(u + 0x20) & 1) { X = a - b; Y = b + a; } else { X = b + a; Y = b - a; }
        uint32_t dx = q34(X), dy = q34(Y);
        if (dx || dy) {
            uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, u) + dy;
            uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, u) + dx;
            e.H(0xc3ee, e.R(u + 0x2c), x, y);
            e.H(0xc376, e.R(u + 0x2c), u, 0);
        }
        e.H(0xc448, u, dx);
        e.H(0xc394, u, dy);
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
