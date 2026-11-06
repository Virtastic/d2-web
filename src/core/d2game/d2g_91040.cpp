// 1.13c D2Game+0x91040: ECX = unit, EDX = seed value, no stack args; returns thunk 0xAFA6's
// result. A wandering path: the point count (thunk 0xAD06, capped at 0x4D through 0xAD42/0xAC7C) is laid out from the
// unit's position in direction steps. The unit seed (+0x20) is reseeded (0xA628 ECX = seed, EDX = value), the path
// (+0x2C) set up (0xAAFC 0xA, 0xAB14 n) and its point buffer fetched (0xAF8E out-pointer). The start is the path's
// +0xC/+0x10 for unit types 2, 4 and 5, else thunks 0xA454/0xA3B8 (0 without a path); the direction is the low byte of
// 0xA7A8(unit, 0xAAC6(path, 0xAAD8(path))). One seed draw picks the turn sign (low bit set: +1, else -1), the next the
// step (lo % 3 + global 0x10F144). Each point adds step*sign to the direction, maps it (& ~0xC0) through thunk 0xAD1E to
// a byte d, adds the dx/dy tables (0xFBA80/0xFBA60, dword d) and stores the 16-bit pair. At every point index that is a
// multiple (signed) of global 0x10F148 the sign flips and a new step is drawn. A null unit returns with nothing done.
#ifndef D2G_91040_CPP
#define D2G_91040_CPP
#include "d2g_mem.h"
namespace d2g_91040 {
using namespace d2g;
struct Env {
    I (*count)(P unit);                    // thunk 0xAD06 (stdcall 1)
    void (*cap1)(P unit, I n);             // thunk 0xAD42 (stdcall 2)
    void (*cap2)(P unit, I n);             // thunk 0xAC7C (stdcall 2)
    void (*reseed)(P seed, U v);           // thunk 0xA628: ECX = seed, EDX = value
    void (*setMode)(P path, I v);          // thunk 0xAAFC (stdcall 2)
    void (*setCount)(P path, I n);         // thunk 0xAB14 (stdcall 2)
    void (*points)(P path, P* out);        // thunk 0xAF8E (stdcall 2): *out = the point buffer
    U (*posX)(P path);                     // thunk 0xA454 (stdcall 1)
    U (*posY)(P path);                     // thunk 0xA3B8 (stdcall 1)
    U (*fAAD8)(P path);                    // thunk 0xAAD8 (stdcall 1)
    U (*fAAC6)(P path, U v);               // thunk 0xAAC6 (stdcall 2)
    U (*dirOf)(P unit, U v);               // thunk 0xA7A8 (stdcall 2)
    U (*mapDir)(U v);                      // thunk 0xAD1E (stdcall 1)
    U (*finish)(P path, I n);              // thunk 0xAFA6 (stdcall 2)
    I (*g)(U rva);                         // module globals 0x10F144 / 0x10F148 and the tables 0xFBA60 / 0xFBA80
};
static inline U step(P seed) {
    const uint64_t v = (uint64_t)rd(seed, 0) * 0x6AC690C5ull + rd(seed, 4);
    wr(seed, 0, (U)v); wr(seed, 4, (U)(v >> 32));
    return (U)v;
}
// 1.13c D2Game+0x91040
inline U WanderPath_113c(const Env& e, P unit, U seedv, U entryEax) {
    if (!unit) return entryEax;
    I n = e.count(unit);
    if (n >= 0x4E) { n = 0x4D; e.cap1(unit, 0x4D); e.cap2(unit, 0x4D); }
    P path = pf(unit, 0x2C);
    P seed = at(unit, 0x20);
    e.reseed(seed, seedv);
    e.setMode(path, 0xA);
    e.setCount(path, n);
    P pts = nullptr;
    e.points(path, &pts);
    const I ty = (I)rd(unit, 0);
    const bool direct = ty == 2 || ty == 4 || ty == 5;
    U x = direct ? rd(pf(unit, 0x2C), 0xC) : (pf(unit, 0x2C) ? e.posX(pf(unit, 0x2C)) : 0);
    U y = direct ? rd(pf(unit, 0x2C), 0x10) : (pf(unit, 0x2C) ? e.posY(pf(unit, 0x2C)) : 0);
    const U a = e.fAAD8(pf(unit, 0x2C));
    const U b = e.fAAC6(pf(unit, 0x2C), a);
    U dir = (uint8_t)e.dirOf(unit, b);
    I sign = (step(seed) & 1) ? 1 : -1;
    U stp = step(seed) % 3 + (U)e.g(0x10F144);
    for (I i = 0; i < n; ++i) {
        dir += stp * (U)sign;
        const U d = (uint8_t)e.mapDir(dir & 0xFFFFFF3Fu);
        x += (U)e.g(0xFBA80 + 4 * d);
        y += (U)e.g(0xFBA60 + 4 * d);
        wr16(pts, 4 * i, (uint16_t)x);
        wr16(pts, 4 * i + 2, (uint16_t)y);
        if (i % e.g(0x10F148) == 0) {
            sign = -sign;
            stp = step(seed) % 3 + (U)e.g(0x10F144);
        }
    }
    return e.finish(path, n);
}
}
#endif
