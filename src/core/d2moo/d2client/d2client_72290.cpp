// 1.13c D2Client+0x72290 -- state-off callback (ECX = unit): for a monster whose path type is 0xA7 and which has a
// pending target position, clears that position and moves the unit there within its room's collision map
// (size 1, mask 0x100).
#include "cl_pa4.h"
extern "C" uint32_t d2c_72290(uint32_t eax, uint32_t u, const cl::Env& e) {
    if (!u) return eax;
    const uint32_t t = e.R(u);
    if (t != 1) return t;
    const uint32_t p = e.H(0xc310, u);
    if (!p) return 0;
    uint32_t r = e.H(0xc42a, p, 0x6FB84CD0u, 0x2f7);
    if (r != 0xa7) return r;
    const uint32_t b = e.H(0xcb6e, p);
    const uint32_t d = e.H(0xcb9e, p);
    if (!b && !d) return 0;
    e.H(0xcb7a, p, 0);
    e.H(0xcb8c, p, 0);
    r = e.H(0x1400, 0, 0, 0, 0, 0, e.H(0xc1f0, u));
    if (!r) return 0;
    r = e.H(0x1400, 0, 0, 0, 0, 0, e.H(0xc1f0, u));
    return e.H(0xcb98, r, b, d, 1, 0x100);
}
