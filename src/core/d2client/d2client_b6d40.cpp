// 1.13c D2Client+0xb6d40 -- client missile: a disc of children of Missiles class `cls` around (x, y).
// EAX = cls, ESI = unit; stack x, y, r, w11, w12, thin, step (ret 0x1C); no return value.
// The row must exist (retail reads row +0x17C unchecked). Block word 1 = the unit itself for unit types 0/1,
// else its room (0xA60D0; none -> nothing). Request 0x201, word 4 = cls, words 11/12 from the stack. The grid
// walks dy then dx from -r to r by `step`, keeping points with dx*dx + dy*dy <= r*r (32-bit wrap). thin > 0 keeps
// a point only when the unit seed (+0x20) rolls 0 mod thin. Row +0x17C: > 0 sets word 16 = roll mod it, < 0 sets
// word 16 = 0. Each point calls ordinal 0xC1F0(unit), then 0x1400 (EAX = that, EBX = x+dx, EDI = y+dy) and
// spawns only when it returns non-zero. The block is reused, so words 5/6/16 carry over.
#include "cl_env.h"
extern "C" void d2c_b6d40(int32_t cls, uint32_t u, int32_t x, int32_t y, int32_t r, uint32_t w11, uint32_t w12,
                          int32_t thin, int32_t step, const cl::Env& e) {
    uint32_t row = e.Row(cls);
    uint32_t q1 = u;
    if (!u || (e.R(u) != 0 && e.R(u) != 1)) {
        q1 = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
        if (!q1) return;
    }
    uint32_t q[23] = {};
    q[0] = 0x201; q[1] = q1; q[4] = (uint32_t)cls; q[11] = w11; q[12] = w12;
    int32_t rr = (int32_t)((uint32_t)r * (uint32_t)r);
    for (int32_t dy = -r; dy <= r; dy += step) {
        uint32_t yy = (uint32_t)dy * (uint32_t)dy;
        for (int32_t dx = -r; dx <= r; dx += step) {
            if ((int32_t)((uint32_t)dx * (uint32_t)dx + yy) > rr) continue;
            if (thin > 0 && e.Lcg(u + 0x20) % (uint32_t)thin) continue;
            q[5] = (uint32_t)(x + dx); q[6] = (uint32_t)(dy + y);
            int32_t ex = (int32_t)e.R(row + 0x17c);
            if (ex) q[16] = ex > 0 ? e.Lcg(u + 0x20) % (uint32_t)ex : 0;
            uint32_t m = e.H(0xc1f0, u);
            if (e.H(0x1400, 0, 0, 0, 0, 0, m, 0, 0, q[5])) e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        }
    }
}
