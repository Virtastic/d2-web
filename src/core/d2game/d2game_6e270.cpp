// 1.13c D2Game+0x6E270. Retail: ECX = game, EDX = unit (non-null: +0x14 is read), plain ret, no result.
// Callees: 0xA478 (stdcall 1: unit, returns the holder), 0xA544 (stdcall 2: unit, &xy), 0xAB86 (stdcall 2: unit, node; int), 0xF42D0 (stdcall 4:
// game, unit, node, 0), 0xDFFD0 (stdcall 2: 0xD, 0), 0xA4A2 (stdcall 2: unit, flag), 0xA520 (stdcall 1: unit+4, returns the record), 0x8EA80
// (ECX = game, EDX = unit, stdcall 5: 3, frame, 0, 0, 0). State byte = unit pData+4: 0 and 1 scan the holder's list (+0x74 / +0xE8) for type-0
// nodes whose mode (+0x10) is not 0x11 and whose AB86 result is below 1; 2 and 3+ send the idle frame (state 2 with a non-zero mode clears it first).
#include <cstdint>
#include <emscripten.h>

struct D2game6e270Env {
    uint32_t (*a478)(uint32_t unit);
    void (*a544)(uint32_t unit, uint32_t* xy);
    int32_t (*ab86)(uint32_t unit, uint32_t node);
    void (*f42d0)(uint32_t game, uint32_t unit, uint32_t node, uint32_t mode);
    void (*fffd0)(uint32_t a, uint32_t b);
    void (*a4a2)(uint32_t unit, uint32_t v);
    uint32_t (*a520)(uint32_t id);
    void (*ea80)(uint32_t game, uint32_t unit, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
};

namespace {
inline uint32_t rd(uint32_t a, uint32_t o) { return *(const uint32_t*)(uintptr_t)(a + o); }
inline uint8_t* stp(uint32_t unit) { return (uint8_t*)(uintptr_t)(rd(unit, 0x14) + 4); }
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2game_6e270(uint8_t* gamep, uint8_t* unitp, const D2game6e270Env* env) {
    const uint32_t g = (uint32_t)(uintptr_t)gamep, unit = (uint32_t)(uintptr_t)unitp;
    const uint32_t mode = rd(unit, 0x10);
    const uint32_t holder = env->a478(unit);
    const uint8_t s = *stp(unit);
    if (s != 0 && s != 1) {
        if (s == 2 && mode != 0) { env->a4a2(unit, 0); *stp(unit) = 0; }
        env->ea80(g, unit, 3, rd(g, 0xA8) + 0xF, 0, 0, 0);
        return;
    }
    const bool flag = s == 1;
    uint32_t xy[2] = { 0, 0 };
    env->a544(unit, xy);
    bool hits = false, found = false;
    for (uint32_t n = rd(holder, 0x74); n; n = rd(n, 0xE8)) {
        if (rd(n, 0) != 0 || rd(n, 0x10) == 0x11) continue;
        if (env->ab86(unit, n) > 0) continue;
        if (flag) { env->f42d0(g, unit, n, 0); hits = true; }
        else { env->fffd0(0xD, 0); found = true; }
    }
    if (found) {
        *stp(unit) = 1;
        env->ea80(g, unit, 3, rd(g, 0xA8) + 0x19, 0, 0, 0);
        return;
    }
    if (!flag) { env->ea80(g, unit, 3, rd(g, 0xA8) + 0xF, 0, 0, 0); return; }
    if (hits) { *stp(unit) = 2; env->a4a2(unit, 1); }
    else *stp(unit) = 0;
    const uint32_t rec = env->a520(rd(unit, 4));
    env->ea80(g, unit, 3, (rd(rec, 0xDC) >> 8) + rd(g, 0xA8) + 1, 0, 0, 0);
}
