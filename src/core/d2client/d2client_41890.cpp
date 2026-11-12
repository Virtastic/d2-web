// 1.13c D2Client+0x41890 -- skill do: marks the unit, resolves the owner slot and the level-derived count, evaluates
// the row's formula at +0x64 and hands all of it to the six-word routine.
#include "cl_skdo.h"
extern "C" uint32_t d2c_41890(uint32_t u, int32_t sk, uint32_t lvl, const cl::Env& e) {
    const uint32_t r = clsk::row(e, sk);
    if (!r) return 0;
    clsk::acted(e, u);
    const int32_t a = (int32_t)e.H(0x40550, u, 0, 0, 0, 0, (uint32_t)sk);
    if (a < 0) return 0;
    const int32_t b = (int32_t)e.H(0x40450, lvl);
    e.Ej(0x40450, (uint32_t)sk);
    if (b <= 0) return 0;
    const uint32_t c = e.H(0xc316, u, e.R(r + 0x64), (uint32_t)sk, lvl);
    e.Xs(lvl);
    e.H(0xa27b0, u, (uint32_t)a, (uint32_t)b, c, (uint32_t)sk);
    return 1;
}
