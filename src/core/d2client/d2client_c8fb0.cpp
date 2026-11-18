// 1.13c D2Client+0xC8FB0 -- skill do: reads the unit's skill-target record (three fields) and hands on to the
// five-word routine (skill, level, and the three fields; EAX = unit).
#include "cl_skdo.h"
extern "C" uint32_t d2c_c8fb0(uint32_t u, int32_t sk, uint32_t lvl, const cl::Env& e) {
    const uint32_t p = e.H(0xc310, u);
    if (!p) return 0;
    const uint32_t x = e.H(0xcb68, p);
    const uint32_t y = e.H(0xcb6e, p);
    const uint32_t z = e.H(0xcb9e, p);
    return e.H(0xc8ce0, (uint32_t)sk, lvl, x, y, z, u);
}
