// 1.13c D2Game+0x6BE40. Retail: ECX = think context (ctx[1] = unit, ctx[2] = target), plain ret, result unused (EAX = 0).
// Callees: 0xA478 (stdcall 1: unit, returns the holder), 0xA55C (stdcall 3: holder, &array, &count), 0x6BD60 (register convention: ECX = matching
// node, ESI = ctx[2], EDI = holder). A holder's unit list is its +0x74, linked through +0xE8; a hit is a type-2 node, not the unit itself,
// whose +4 equals the unit's +4 (-1 for a null unit). The own holder is scanned first, then every other holder of the array.
#include <cstdint>
#include <emscripten.h>

struct D2game6be40Env {
    uint32_t (*a478)(uint32_t unit);
    void (*a55c)(uint32_t holder, uint32_t* arr, uint32_t* cnt);
    void (*s6bd60)(uint32_t node, uint32_t target, uint32_t holder);
};

namespace {
inline uint32_t rd(uint32_t a, uint32_t o) { return *(const uint32_t*)(uintptr_t)(a + o); }
inline uint32_t hit(uint32_t holder, uint32_t unit, uint32_t id) {
    for (uint32_t n = rd(holder, 0x74); n; n = rd(n, 0xE8)) if (rd(n, 0) == 2 && n != unit && rd(n, 4) == id) return n;
    return 0;
}
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2game_6be40(const uint8_t* ctx, const D2game6be40Env* env) {
    const uint32_t unit = ((const uint32_t*)ctx)[1], target = ((const uint32_t*)ctx)[2];
    const uint32_t owner = env->a478(unit);
    if (!owner) return;
    const uint32_t id = unit ? rd(unit, 4) : 0xFFFFFFFFu;
    if (uint32_t n = hit(owner, unit, id)) { env->s6bd60(n, target, owner); return; }
    uint32_t arr = 0, cnt = 0;
    env->a55c(owner, &arr, &cnt);
    for (uint32_t i = 0; i < cnt; ++i) {
        const uint32_t h = rd(arr, i * 4);
        if (h == owner) continue;
        if (uint32_t n = hit(h, unit, id)) { env->s6bd60(n, target, h); return; }
    }
}
