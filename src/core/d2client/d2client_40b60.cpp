// 1.13c D2Client+0x40B60 -- skill do: the two-step presentation with the first step on the unit's path target and the
// second on the unit itself, fed the row's graded pair. A unit without a path is reported through the error routines.
#include "cl_skdo.h"
extern "C" uint32_t d2c_40b60(uint32_t u, int32_t sk, uint32_t, const cl::Env& e) {
    const uint32_t r = clsk::row(e, sk);
    if (!r) return 0;
    e.H(0x2a0b0, 0, 0, 0, 0, 0, u);
    uint32_t w = e.R(u + 0x2c);
    if (!w) {
        const uint32_t a = e.H(0xbf92, 0x946);
        e.H(0xbf80, 0x6FB8359Fu, a);
        w = e.H(0x3664, 0xffffffffu);
    }
    const uint32_t t = e.H(0xc2da, w);
    if (!t) return 0;
    int32_t a, b;
    clsk::gradePair(e, u, r, a, b);
    e.Xs(0, 0);
    e.H(0x6b300, t, 2, 0, 0, 0, (uint32_t)a);
    e.H(0x88a70, u, 0, 0, 0, 0, 0, 0, 0, (uint32_t)b);
    return 1;
}
