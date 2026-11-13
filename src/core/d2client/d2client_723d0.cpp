// 1.13c D2Client+0x723D0 -- state callback (ECX = unit, EDX = state): starts the screen fade from the state's skill.
// The skill's calc at +0x60 gives a length v; the fade record gets 3v/5 and 2v/5 (0x19 when that is 0) as its two
// phases with a 5-tick gap, their sum as the total and the unit's level id. Nothing is written while a fade runs.
#include "cl_pa4.h"
extern "C" uint32_t d2c_723d0(uint32_t u, uint32_t st, const cl::Env& e) {
    using namespace pa4;
    const uint32_t rm = e.H(0xc1f0, u);
    if (!rm) return 0;
    const uint32_t s = e.H(0xc1c0, u, st);
    if (!s) return 0;
    const uint32_t a = e.H(0xc4b4, s, 0x15e, 0);
    const uint32_t b = e.H(0xc4b4, s, 0x15f, 0);
    const uint32_t row = skRow(e, a);
    if (!row) return b;
    const int32_t v = (int32_t)e.H(0xc316, u, e.R(row + 0x60), a, b);
    const uint32_t lvl = e.H(0xc1de, rm);
    int32_t si = (int32_t)((uint32_t)v * 2) / 5;
    const int32_t bx = (int32_t)((uint32_t)v * 3) / 5;
    const uint32_t busy = G(e, 0x119634);
    if (busy) return busy;
    for (uint32_t i = 0; i < 11; ++i) S(e, 0x11960c + 4 * i, 0);
    S(e, 0x11960c, 1);
    S(e, 0x119618, 5);
    S(e, 0x119620, (uint32_t)bx);
    if (!si) si = 0x19;
    const uint32_t tot = (uint32_t)bx + (uint32_t)si + 5;
    S(e, 0x11961c, (uint32_t)si);
    S(e, 0x119624, tot);
    S(e, 0x119630, lvl);
    return tot;
}
