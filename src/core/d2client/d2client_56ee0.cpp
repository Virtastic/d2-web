// 1.13c D2Client+0x56EE0 -- roll a skill's cast duration (fastcall ECX = unit, EDX = skill; stack p1; ret 4). Skill out of the
// skill-desc table (+0xB98 / +0xBA0, stride 0x23C): 0. g = 0xC310 (unit), none: 0. 0x2A0B0 (EAX = unit). A unit without a path
// is fatal in retail (0xBF92 / 0xBF80 / 0x3664 (-1)), not reached here. q = 0xC2DA (path), none: 0. Seed: 0xC634 (ECX = &seed),
// 0xC3B2 (ECX = &seed, EDX = q+0xC + unit+0xC). lo / hi = 0xC316 (unit, skrow+0x138 / +0x13C, skill, p1);
// v = 0x1380 (ECX = &seed, ESI = hi - lo) + lo, at least 1. 0xCB50 (g, v), 0xCB7A (g, q+0xC), 0xCB8C (g, q+0); 1.
#include "cl_env.h"
extern "C" uint32_t d2c_56ee0(uint32_t unit, uint32_t skill, uint32_t p1, const cl::Env& e) {
    if ((int32_t)skill < 0 || (int32_t)skill >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    uint32_t sk = e.R(e.dt + 0xb98) + skill * 0x23c;
    if (!sk) return 0;
    uint32_t g = e.H(0xc310, unit);
    if (!g) return 0;
    e.H(0x2a0b0, 0, 0, 0, 0, 0, unit);
    uint32_t path = e.R(unit + 0x2c);
    if (!path) { e.H(0xbf92, 0x946); return 0; }   // retail: fatal error, no return
    uint32_t q = e.H(0xc2da, path);
    if (!q) return 0;
    uint32_t seed[2] = {};
    uint32_t ps = (uint32_t)(uintptr_t)seed;
    e.H(0xc634, 0, 0, 0, 0, 0, 0, ps);
    e.H(0xc3b2, 0, 0, 0, 0, 0, 0, ps, e.R(q + 0xc) + e.R(unit + 0xc));
    int32_t lo = (int32_t)e.H(0xc316, unit, e.R(sk + 0x138), skill, p1);
    int32_t hi = (int32_t)e.H(0xc316, unit, e.R(sk + 0x13c), skill, p1);
    int32_t v = (int32_t)e.H(0x1380, 0, 0, 0, 0, 0, 0, ps, 0, 0, (uint32_t)(hi - lo)) + lo;
    if (v < 1) v = 1;
    e.H(0xcb50, g, (uint32_t)v);
    e.H(0xcb7a, g, e.R(q + 0xc));
    e.H(0xcb8c, g, e.R(q));
    return 1;
}
