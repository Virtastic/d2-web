// D2Common 0x12330 (#10314), retail layout: the unit's room (0x2FE10) carries the near-room
// array at +0x00 and its count at +0x24; each near room passing the 0x121D0 box test (margin = range, or 0x10000 when
// range is 0) has its unit list walked (+0x74, next +0xE8); the nearest unit by 0x7F5E0 distance below the margin and the
// best so far (initially 0xFFFF) that the filter accepts is returned. Null room / bad filter are fatal (0x90 / 0x9F): 0.
#include <cstdint>
#include <cstring>
#include "d2common_12330.h"
static uint32_t Rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
// 1.13c D2Common+0x12330
extern "C" uint32_t D2Common_12330_FindNearest(uint32_t unit, int32_t x, int32_t y, int32_t range, const D2Common12330Env* env) {
    const uint32_t room = env->room(unit);
    if (!room || !env->filter) return 0;
    const int32_t n = (int32_t)Rd(room, 0x24);
    const uint32_t arr = Rd(room, 0);
    const int32_t margin = range ? range : 0x10000;
    uint32_t best = 0; int32_t bestD = 0xffff;
    for (int32_t i = 0; i < n; ++i) {
        if (!env->inBox(Rd(arr, 4 * i), x, y, margin)) continue;
        for (uint32_t u = Rd(Rd(arr, 4 * i), 0x74); u; u = Rd(u, 0xe8)) {
            const int32_t d = env->dist(u, x, y);
            if (d < margin && d < bestD && env->filter(u, unit)) { bestD = d; best = u; }
        }
    }
    return best;
}
