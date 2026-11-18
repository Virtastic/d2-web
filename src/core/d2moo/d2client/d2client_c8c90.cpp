// 1.13c D2Client+0xC8C90 -- skill do: reads the unit's skill-target record (x, y, and a third field that is fetched
// but not used) and hands on to the targeted routine (stack: unit, level, x; ECX = y, EDX = skill).
#include "cl_skdo.h"
extern "C" uint32_t d2c_c8c90(uint32_t u, int32_t sk, uint32_t lvl, const cl::Env& e) {
    const uint32_t p = e.H(0xc310, u);
    if (!p) return 0;
    const uint32_t x = e.H(0xcb68, p);
    const uint32_t y = e.H(0xcb6e, p);
    e.H(0xcb9e, p);
    return e.H(0xc8710, u, lvl, x, 0, 0, 0, y, (uint32_t)sk);
}
