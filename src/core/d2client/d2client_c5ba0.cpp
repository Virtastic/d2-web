// 1.13c D2Client+0xC5BA0 -- skill do: a timed missile. While the frame counter is past the target record's first
// field the unit is sent back through the three-word routine; otherwise the missile is made, primed and given the
// row's formula value, its draw record is flagged and the unit's acted flag is cleared.
#include "cl_skdo.h"
extern "C" uint32_t d2c_c5ba0(uint32_t u, int32_t sk, uint32_t lvl, const cl::Env& e) {
    const uint32_t r = clsk::row(e, sk);
    if (!r) return 0;
    const uint32_t p = e.H(0xc310, u);
    if (!p) return 0;
    const uint32_t now = clc::G(e, 0x1197F8);
    if (now > e.H(0xcb68, p)) {
        e.H(0xc30a, u, 0xc, 0);
        return 0;
    }
    const uint32_t m = e.H(0x74930, (uint32_t)sk, lvl, 0, 0, 0, u);
    if (!m) return 0;
    e.H(0xc7b00, 0, 0, 0, 0, 0, m);
    const uint32_t v = e.H(0xc316, u, e.R(r + 0x114), (uint32_t)sk, lvl);
    e.H(0xc7e30, 0, 0, 0, 0, 0, m, v, 0);
    const uint32_t mm = e.R(m + 0x54);
    if (mm) {
        const uint32_t q = e.R(mm + 0x30);
        if (q) e.W(q, e.R(q) | 0x20);
    }
    if (u) e.W(u + 0xc4, e.R(u + 0xc4) & ~0x40u);
    return 1;
}
