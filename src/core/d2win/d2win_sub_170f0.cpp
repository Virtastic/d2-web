// src/core/d2win/d2win_sub_170f0.cpp -- 1.13c D2Win.dll sub_170F0 from the cached Ghidra decompile.
#include <cstdint>

extern uint32_t d2win_g_6f901488, d2win_g_6f90148c;   // mouse x / y, d2win_batch_10030.cpp

// Per-axis distance from the mouse to a control rect: x = [+0xC,+0xC+w[+0x14]], y = (+0x10-h[+0x18], +0x10].
static void d2win_170f0_dist(const int32_t* o, int32_t& dx, int32_t& dy) {
    const int32_t mx = (int32_t)d2win_g_6f901488, my = (int32_t)d2win_g_6f90148c;
    int32_t x = o[3], y = o[4];
    if (mx < x) dx = mx - x;
    else if (o[5] + x < mx) dx = (mx - o[5]) - x;
    else dx = 0;
    if (my < y - o[6]) dy = (o[6] - y) + my;
    else if (y < my) dy = my - y;
    else dy = 0;
}

// 1.13c D2Win+0x170F0
// Picks the control nearer the mouse: returns a unless b is nearer. Null a -> b; null b -> a.
extern "C" intptr_t d2win_sub_170F0(intptr_t a, intptr_t b) {
    if (a) {
        if (!b) return a;
        int32_t ax, ay, bx, by;
        d2win_170f0_dist((const int32_t*)a, ax, ay);
        d2win_170f0_dist((const int32_t*)b, bx, by);
        const bool ain = ay == 0 && ax < 1, bin = by == 0 && bx < 1;
        if (ain) {
            if (!bin || bx < ax) return a;
        } else if (!bin && ay * ay + ax * ax < by * by + bx * bx) return a;
    }
    return b;
}
