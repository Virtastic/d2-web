// 1.13c D2Game+0x6E0E0. Retail: ECX = think context (ctx[0] game, [1] unit, [2] target, [4] monster id), plain ret, result 0.
// A unit with a non-zero mode (+0x10) returns at once. Otherwise 0xF42D0 (stdcall 4: game, unit, target, mode) with mode 0 then 1,
// 0xA4A2(unit, 1), then the mode-1 event through 0x8EA80 (ECX = game, EDX = unit, stdcall 5: 1, (rec+0xDC >> 8) + 1 + game+0xA8, 0, 0, 0)
// with rec from 0xA520(ctx[4]). A null unit takes the full path.
#include <cstdint>
#include <emscripten.h>

struct D2game6e0e0Env {
    void (*f42d0)(uint32_t game, uint32_t unit, uint32_t target, uint32_t mode);
    void (*a4a2)(uint32_t unit, uint32_t v);
    uint32_t (*a520)(uint32_t id);
    void (*ea80)(uint32_t game, uint32_t unit, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
};

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2game_6e0e0(const uint8_t* ctxp, const D2game6e0e0Env* env) {
    const uint32_t* ctx = (const uint32_t*)ctxp;
    if (ctx[1] && *(const uint32_t*)(uintptr_t)(ctx[1] + 0x10)) return 0;
    env->f42d0(ctx[0], ctx[1], ctx[2], 0);
    env->f42d0(ctx[0], ctx[1], ctx[2], 1);
    env->a4a2(ctx[1], 1);
    const uint32_t rec = env->a520(ctx[4]);
    env->ea80(ctx[0], ctx[1], 1, (*(const uint32_t*)(uintptr_t)(rec + 0xDC) >> 8) + 1 + *(const uint32_t*)(uintptr_t)(ctx[0] + 0xA8), 0, 0, 0);
    return 0;
}
