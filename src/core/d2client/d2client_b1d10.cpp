// 1.13c D2Client+0xb1d10 -- client missile: tethered to a type-3 unit (orbit / flee step). ECX = unit, no return.
// t = 0xC4BA(unit). No path (+0x2C): the 0xB17C0 tail. k = 0xC322(unit); the type-3 client hash at 0x10A008 (bucket k & 0x7F,
// chain +0xE4, key +0xC); not found: tail. A found unit of another type asserts (0xBF92(0x5A), 0xBF80, exit). v = 0xC322(found).
// t != v and v >= 2 and 0x1330(EAX = 0x19, ECX = seed) == 0: 0xC394(unit, v); v == 2: 0xC328(path, 0xF00, file, 0x19DB),
// 0xC3EE(path, 0x3B1F, 0x170C), 0xC376(path, unit, 0); tail. Otherwise t == 0: LCG(seed) % 10 == 0 gives 0xC394(unit, 1),
// 0xC7BF0(EAX = unit), 0xC328(path, 0xF00, file, 0x19E6) and, when 0x1330(EAX = 2) != 0, motion (+0x54 -> +0x30) +0x10 = +0x14 = 0,
// +0x18 = 0x800. t == 1: d = unit - found (x, y via 0x1630 / 0x1660, the found unit first), negated when byte +0xC & 2; non-zero:
// 0xC3EE(path, x(unit) - dy, y(unit) + dx) (y first), 0xC376(path, unit, 0). Always the 0xB17C0(ECX = unit) tail.
#include "cl_env.h"
extern "C" void d2c_b1d10(uint32_t u, const cl::Env& e) {
    uint32_t seed = u + 0x20;
    uint32_t t = e.H(0xc4ba, u);
    uint32_t path = e.R(u + 0x2c);
    if (!path) { e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u); return; }
    uint32_t k = e.H(0xc322, u);
    uint32_t f = e.R(e.gl + (k & 0x7f) * 4);   // 0x10A008
    while (f && e.R(f + 0xc) != k) f = e.R(f + 0xe4);
    if (!f) { e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u); return; }
    if (e.R(f) != 3) {
        uint32_t s = e.H(0xbf92, 0x5a);
        e.H(0xbf80, 0xB2000000u | 0xd359fu, s);
        e.H(0x3664, 0xffffffffu);
        return;
    }
    int32_t v = (int32_t)e.H(0xc322, f);
    if ((int32_t)t != v && v >= 2 && !e.H(0x1330, 0, 0, 0, 0, 0, 0x19, seed)) {
        e.H(0xc394, u, (uint32_t)v);
        if (v == 2) {
            e.H(0xc328, path, 0xf00, 0xB2000000u | 0xd4010u, 0x19db);
            e.H(0xc3ee, path, 0x3b1f, 0x170c);
            e.H(0xc376, path, u, 0);
        }
    } else if (t == 0) {
        if (e.Lcg(seed) % 10u == 0) {
            e.H(0xc394, u, 1);
            e.H(0xc7bf0, 0, 0, 0, 0, 0, u);
            e.H(0xc328, path, 0xf00, 0xB2000000u | 0xd4010u, 0x19e6);
            if (e.H(0x1330, 0, 0, 0, 0, 0, 2, seed)) {
                uint32_t pa = e.R(u + 0x54);
                uint32_t mo = pa ? e.R(pa + 0x30) : 0;
                if (mo) { e.W(mo + 0x10, 0); e.W(mo + 0x14, 0); e.W(mo + 0x18, 0x800); }
            }
        }
    } else if (t == 1) {
        uint32_t fx = e.H(0x1630, 0, 0, 0, 0, 0, 0, f);
        uint32_t dx = e.H(0x1630, 0, 0, 0, 0, 0, 0, u) - fx;
        uint32_t fy = e.H(0x1660, 0, 0, 0, 0, 0, 0, f);
        uint32_t dy = e.H(0x1660, 0, 0, 0, 0, 0, 0, u) - fy;
        if (e.R8(u + 0xc) & 2) { dx = 0u - dx; dy = 0u - dy; }
        if (dx || dy) {
            uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, u) + dx;
            uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, u) - dy;
            e.H(0xc3ee, path, x, y);
            e.H(0xc376, path, u, 0);
        }
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
