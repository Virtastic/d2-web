// 1.13c D2Client+0xb75c0 -- client missile: periodic skill-ranged child. ECX = unit, no return.
// No unit / class / row, class word row +0x1E < 0, no room (0xA60D0), D2Common 0xC1F0(u) == 0, or no skill row
// (0x15B0, EAX = skill 0xC388(u)): 0xB1720() and done. lv = 0xC3FA(u); d1 = 0xC316(room, srow +0x138, skill, lv),
// d2 = max(0xC316(room, srow +0x13C, skill, lv), 1) (the room re-fetched for each). t = 0xC4D2(u); t not a multiple
// of d2 goes to the tail. Seed +0x20 := 0xC3B2(ECX = seed, EDX = 0x1630(u) + t); spot = 0x1380(ECX = seed,
// ESI = 2 * (d1 - 1)) - (d1 - 1) + 0x1630 / 0x1660(u) per axis. 0xC4A8(c1f0, x, y, 0x45) (word) != 0 goes to the tail.
// Request 0x2001, room, class row +0x1E at the spot (word 2 left 0); a spawned missile gets +0xC4 |= 0x10000; then
// class row +0x20 when >= 0 with the same block. Tail: 0xB17C0(ECX = u).
#include "cl_env.h"
extern "C" void d2c_b75c0(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u), room = 0, c1 = 0, srow = 0, sk = 0;
    if (row && e.R16s(row + 0x1e) >= 0 && (room = e.H(0xa60d0, 0, 0, 0, 0, 0, u)) && (c1 = e.H(0xc1f0, u))) {
        sk = e.H(0xc388, u);
        srow = e.H(0x15b0, 0, 0, 0, 0, 0, sk);
    }
    if (!srow) { e.H(0xb1720); return; }
    uint32_t lv = e.H(0xc3fa, u);
    uint32_t a = e.R(srow + 0x138);
    int32_t d1 = (int32_t)e.H(0xc316, e.H(0xa60d0, 0, 0, 0, 0, 0, u), a, sk, lv);
    uint32_t b = e.R(srow + 0x13c);
    int32_t d2 = (int32_t)e.H(0xc316, e.H(0xa60d0, 0, 0, 0, 0, 0, u), b, sk, lv);
    if (d2 < 1) d2 = 1;
    int32_t t = (int32_t)e.H(0xc4d2, u);
    if (t % d2 == 0) {
        uint32_t seed = u + 0x20;
        e.H(0xc3b2, 0, 0, 0, 0, 0, 0, seed, e.H(0x1630, 0, 0, 0, 0, 0, 0, u) + (uint32_t)t);
        uint32_t h = (uint32_t)d1 - 1, w = h + h;
        uint32_t px = e.H(0x1380, 0, 0, 0, 0, 0, 0, seed, 0, 0, w) - h;
        px += e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
        uint32_t py = e.H(0x1380, 0, 0, 0, 0, 0, 0, seed, 0, 0, w) - h;
        py += e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
        if (!(e.H(0xc4a8, c1, px, py, 0x45) & 0xffff)) {
            uint32_t q[23] = {};
            q[0] = 0x2001; q[1] = room;
            q[11] = e.H(0xc388, u);
            q[12] = e.H(0xc3fa, u);
            q[4] = (uint32_t)e.R16s(row + 0x1e);
            q[5] = px; q[6] = py;
            uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
            if (m) e.W(m + 0xc4, e.R(m + 0xc4) | 0x10000);
            int32_t c2 = e.R16s(row + 0x20);
            if (c2 >= 0) { q[4] = (uint32_t)c2; e.HB(0xb30f0, (uint32_t)(uintptr_t)q); }
        }
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
