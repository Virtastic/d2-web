// 1.13c D2Client+0x72320 -- state callback (ECX = unit, EDX = state) for another player's summon-count state: reads
// the state's two stats (0x15E = skill, 0x15F = wanted count), compares the wanted count with the unit's current
// count for that skill and, when they differ, sets the skill's level: capped at the skill's maximum, with the
// excess applied through the bonus routine.
#include "cl_pa4.h"
extern "C" uint32_t d2c_72320(uint32_t u, uint32_t st, const cl::Env& e) {
    using namespace pa4;
    const uint32_t pl = G(e, 0x11bbfc);
    if (u == pl) return pl;
    const uint32_t s = e.H(0xc1c0, u, st);
    if (!s) return 0;
    const uint32_t b = e.H(0xc4b4, s, 0x15e, 0);
    const uint32_t n = e.H(0xc4b4, s, 0x15f, 0);
    const uint32_t r = e.H(0xc4ea, 0xFFFFFFFFu, 0, 0, 0, 0, 0, u, b);
    const uint32_t cur = r ? e.H(0xc718, u, r, 1) : 0;
    if (cur == n) return cur;
    const uint32_t m = e.H(0x17b0, 0, 0, 0, 0, 0, b);
    if ((int32_t)n > (int32_t)m) {
        e.H(0xc550, u, b, m, 0);
        xw(e, 0xc550, {0x6FB84CD0u, 0x11b});
        return e.H(0xc71e, u, b, n - m);
    }
    const uint32_t a = e.H(0xc550, u, b, n, 0);
    xw(e, 0xc550, {0x6FB84CD0u, 0x120});
    return a;
}
