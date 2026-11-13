// 1.13c D2Client+0xB05B0 (ecx = unit, edx = skip): returns 0 when edx is non-zero. Otherwise the unit's 0xC37C value picks an effect id (0x110 gives 0x158;
// 0x111, 0x112, 0x1A1 give 0x159; anything else -1), the unit's position is read as in 0xB0870 (path +0x10 / +0xC for the direct unit types, else
// 0xC2EC / 0xC3BE), and 0xAFF10 creates the effect in the 0xA60D0 room; a created unit takes the parent's 0xC2D4 value (0xC3C4) and 0xC2FE(unit, 0x80).
// Returns 1.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB05B0
extern "C" uint32_t d2c_b05b0(uint32_t u, uint32_t skip, const cl::Env& e) {
    if (skip) return 0;
    uint32_t t = e.H(0xc37c, u);
    uint32_t kind = 0xffffffffu;
    if (t == 0x110) kind = 0x158;
    else if (t == 0x111 || t == 0x112 || t == 0x1a1) kind = 0x159;
    uint32_t ty = e.R(u), p = e.R(u + 0x2c);
    bool direct = ty == 2 || ((int32_t)ty > 3 && (int32_t)ty <= 5);
    uint32_t bx, di;
    if (direct) { bx = e.R(p + 0x10); di = e.R(p + 0xc); }
    else { bx = p ? e.H(0xc2ec, p) : 0; di = p ? e.H(0xc3be, p) : 0; }
    uint32_t k = e.H(0xc3fa, u);
    uint32_t g = e.H(0xc388, u);
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    uint32_t nu = e.H(0xaff10, kind, di, bx, g, k, 0, 0, room, 0, u);
    if (!nu) return 1;
    uint32_t q = e.R(nu + 0x2c);
    uint32_t a = e.H(0xc2d4, e.R(u + 0x2c));
    e.H(0xc3c4, q, a);
    e.H(0xc2fe, nu, 0x80);
    return 1;
}
