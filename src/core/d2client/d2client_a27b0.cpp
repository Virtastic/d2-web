// 1.13c D2Client+0xA27B0 (411 B, stdcall, ret 0x18): scatter_effects. Stack: unit, a1, count,
// spread, a4, a5. 0xA1340 (EBX = unit, EDI = &x, stack &y) gives the base position; 0 there returns 0. 0xC3B2 (ECX = unit+0x20
// seed, EDX = x) then, per effect, two rolls of the unit seed (lo*0x6AC690C5 + hi; mask when 2*spread is a power of two, else
// unsigned mod; 0 when 2*spread < 1) offset x and y by roll - spread. Size: unit types 2, 4, 5 read path +0x10 / +0xC, others
// call 0xC2EC / 0xC3BE on the path (0 without one). 0xC6FA(h, w, x, y) >= 4 spawns via 0xB9A80 (EAX = unit, 8 stack words
// a1, 0, 0, x, y, a4, a5, 0; the hookblk logs all eight). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_a27b0(uint32_t u, uint32_t a1, int32_t count, int32_t spread, uint32_t a4, uint32_t a5, const cl::Env& e) {
    uint32_t xy[2] = { 0, 0 };   // x (EDI out), y (stack out)
    const uint32_t px = (uint32_t)(uintptr_t)&xy[0], py = (uint32_t)(uintptr_t)&xy[1];
    if (e.ed) e.ed(e.ctx, px);
    if (!e.H(0xa1340, py, 0, 0, 0, 0, 0, 0, 0, u)) return 0;
    const uint32_t seed = u + 0x20;
    e.H(0xc3b2, 0, 0, 0, 0, 0, 0, seed, xy[0]);
    const uint32_t n2 = (uint32_t)spread * 2;
    auto roll = [&]() -> uint32_t {
        if ((int32_t)n2 < 1) return 0;
        const uint32_t lo = e.Lcg(seed);
        return (n2 & (n2 - 1)) ? lo % n2 : (n2 - 1) & lo;
    };
    for (int32_t i = count; i > 0; --i) {
        const uint32_t x = roll() - (uint32_t)spread + xy[0];
        const uint32_t y = roll() - (uint32_t)spread + xy[1];
        const uint32_t ty = e.R(u), path = e.R(u + 0x2c);
        const bool fixed = ty == 2 || ((int32_t)ty > 3 && (int32_t)ty <= 5);
        const uint32_t w = fixed ? e.R(path + 0x10) : path ? e.H(0xc2ec, path) : 0;
        const uint32_t h = fixed ? e.R(path + 0xc) : path ? e.H(0xc3be, path) : 0;
        if ((int32_t)e.H(0xc6fa, h, w, x, y) >= 4) {
            uint32_t s[8] = { a1, 0, 0, x, y, a4, a5, 0 };
            e.H(0xb9a80, s[0], s[1], s[2], s[3], s[4], u);
            if (e.blk) e.blk(e.ctx, 0xb9a80, (uint32_t)(uintptr_t)s, 8);
        }
    }
    return 1;
}
