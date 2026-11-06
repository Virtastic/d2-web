// 1.13c D2Game+0x2F690: object init callback that scatters trapped-soul monsters (class
// 0x193 / 0x194) around a room. ECX = the init block (+0 game, +4 object unit, +8 room, +0xC seed pointer, +0x14 x,
// +0x18 y), plain ret. Nothing happens when the object's +0x10 word is set. The room rectangle (thunk 0xA6D0: x0, y0,
// w, h) sets the budget n = ((w*h >> 7) * 30) >> 8; up to 12 rounds while n > 0: a random point inside the rect (game
// seed +0x10F0, rnd(w-4), rnd(h-4)) is tested with 0xD2E660-style placement 0x2E660 (EAX = rect, ESI = point, EDI = 3;
// stack room, 3); on a fit one monster is spawned at the object's position (0xEEF10, ECX = a 10-field spawn block),
// then a chain grows from the point: each link takes up to 3*max(n,4) random steps (direction tables 0x10E00C /
// 0x10E02C, step (r%5+5)*2) until a step fits, and spawns there; link k continues only while rnd(k>>1) is 0.
// Every successful spawn calls thunk 0xA4A2(object, 2) and spends one of n.
#ifndef D2G_2F690_CPP
#define D2G_2F690_CPP
#include "d2g_mem.h"
namespace d2g_2f690 {
using namespace d2g;
struct Env {
    void (*roomRect)(P room, U* rect);             // thunk 0xA6D0 (stdcall 2): fills x0, y0, w, h
    I (*fits)(U* rect, U* pt, I sx, P room, I sy);  // 0x2E660: EAX = rect, ESI = point, EDI = sx; stack room, sy; ret 8
    P (*spawn)(U* blk);                            // 0xEEF10: ECX = spawn block (10 dwords, the last a word)
    void (*touch)(P obj, I v);                     // thunk 0xA4A2 (stdcall 2)
    I (*dx)(U d);                                  // dword table 0x10E00C[d]
    I (*dy)(U d);                                  // dword table 0x10E02C[d]
};
static inline U step(P s) {
    const uint64_t v = (uint64_t)rd(s, 0) * 0x6AC690C5ull + rd(s, 4);
    wr(s, 0, (U)v); wr(s, 4, (U)(v >> 32));
    return (U)v;
}
// the inline roll: 0 for n <= 0, a mask for a power of two, else the remainder
static inline U roll(P s, I n) {
    if (n <= 0) return 0;
    const U v = step(s);
    return ((U)n & (U)(n - 1)) == 0 ? (v & (U)(n - 1)) : v % (U)n;
}
// 1.13c D2Game+0x2F690
inline void TrappedSouls_113c(const Env& e, P ctx) {
    P obj = pf(ctx, 4);
    if (obj && rd(obj, 0x10)) return;
    U rect[8];
    e.roomRect(pf(ctx, 8), rect);
    P seed = pf(pf(ctx, 0), 0x10F0);
    I n = (((I)(rect[3] * rect[2]) >> 7) * 0x1E) >> 8;
    auto spawnAt = [&](U x, U y) {
        const U cls = (step(pf(ctx, 0xC)) & 1) ? 0x194 : 0x193;
        U b[10] = { rd(ctx, 0), rd(ctx, 8), 0, cls, 1, 0, x, y, 0xFFFFFFFFu, 0 };
        if (e.spawn(b)) { e.touch(pf(ctx, 4), 2); --n; }
    };
    if (n <= 0) return;
    for (I outer = 0xC; outer > 0; ) {
        I x = (I)roll(seed, (I)rect[2] - 4) + (I)rect[0];
        I y = (I)roll(seed, (I)rect[3] - 4) + (I)rect[1];
        U pt[2] = { (U)x, (U)y };
        if (e.fits(rect, pt, 3, pf(ctx, 8), 3) == 1) {
            spawnAt(rd(ctx, 0x14), rd(ctx, 0x18));
            I ok = 1, k = 1;
            for (;;) {
                const I m = k >> 1;
                if (m > 0 && roll(seed, m) != 0) break;
                if (!ok) break;
                ok = 0;
                const I lim = 3 * (n < 4 ? 4 : n);
                for (I cnt = 0; cnt < lim && !ok; ++cnt) {
                    const U v1 = step(seed), d = v1 & 7;
                    const U r1 = step(seed) % 5, r2 = step(seed) % 5;
                    x += (I)(r1 + 5) * e.dx(d) * 2;
                    y += (I)(r2 + 5) * e.dy(d) * 2;
                    pt[0] = (U)x; pt[1] = (U)y;
                    ok = e.fits(rect, pt, 3, pf(ctx, 8), 3);
                }
                if (!ok) continue;
                ++k;
                spawnAt((U)x, (U)y);
            }
        }
        --outer;
        if (n <= 0) break;
    }
}
}
#endif
