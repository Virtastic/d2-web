// 1.13c D2Client+0xA2380: ECX = count (signed), ESI = h (seeded owner, LCG at +0x20/+0x24), EDI = p (request block), one stack
// argument (spread), ret 4. Steps h's seed once for the starting corner, then count times scatters the block's position by
// rolls in [0, 2*spread), sets the corner offset at +0x1C/+0x20, the callback tag at +0x54, the corner index << 4 at +0x58 and runs 0xB30F0.
#include "cl_env.h"
static uint32_t rollf(const cl::Env& e, uint32_t h, int32_t n2) {
    if (n2 <= 0) return 0;
    const uint32_t lo = e.Lcg(h + 0x20);
    if (!((uint32_t)n2 & ((uint32_t)n2 - 1))) return ((uint32_t)n2 - 1) & lo;
    return lo % (uint32_t)n2;
}
extern "C" void d2cl_a2380(int32_t count, uint32_t h, uint32_t p, int32_t spread, const cl::Env& e) {
    uint32_t dir = e.Lcg(h + 0x20) & 3;
    const uint32_t y0 = e.R(p + 0x18), x0 = e.R(p + 0x14);
    const int32_t n2 = (int32_t)((uint32_t)spread * 2);
    static const int32_t cx[4] = { -0x10, -0x10, 0x10, 0x10 }, cy[4] = { -0x10, 0x10, 0x10, -0x10 };
    for (int32_t i = 0; i < count; ++i) {
        const int32_t lx = cx[dir], ly = cy[dir];
        dir = (dir + 1) & 3;
        const uint32_t r1 = rollf(e, h, n2);
        e.W(p + 0x14, r1 - (uint32_t)spread + x0);
        const uint32_t r2 = rollf(e, h, n2);
        const uint32_t y = y0 + (r2 - (uint32_t)spread);
        e.W(p + 0x18, y);
        e.W(p + 0x20, y + (uint32_t)ly);
        e.W(p + 0x1c, e.R(p + 0x14) + (uint32_t)lx);
        e.W(p + 0x54, 0xB2000000u | 0xa0650u);
        e.W(p + 0x58, dir << 4);
        e.HB(0xb30f0, p);
    }
}
