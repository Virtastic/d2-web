// 1.13c D2Game+0x6DE00. Retail: ECX = think context (ctx[0] game, [1] unit, [2] target, [4] monster id), plain ret, result 1.
// A unit with a non-zero mode (+0x10) returns at once. Otherwise: 0xA4A2(unit, 1) and 0xA550(unit); the monster record (0xA520, stdcall 1,
// returned three times) byte at +0xC4 + mode picks unit flag bit 2 (set / cleared); a non-zero record byte +0x141 sends the mode-1 event
// through 0x8EA80 (ECX = game, EDX = unit, stdcall 5: 1, (rec+0xDC >> 8) + 1 + game+0xA8, 0, 0, 0). A null unit skips the flag write.
#include <cstdint>
#include <emscripten.h>

struct D2game6de00Env {
    void (*a4a2)(uint32_t unit, uint32_t v);
    void (*a550)(uint32_t unit);
    uint32_t (*a520)(uint32_t id);
    void (*ea80)(uint32_t game, uint32_t unit, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
};

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2game_6de00(const uint8_t* ctxp, const D2game6de00Env* env) {
    const uint32_t* ctx = (const uint32_t*)ctxp;
    uint8_t* unit = (uint8_t*)(uintptr_t)ctx[1];
    if (unit && *(uint32_t*)(unit + 0x10)) return 1;
    env->a4a2(ctx[1], 1);
    env->a550(ctx[1]);
    const uint32_t mode = unit ? *(uint32_t*)(unit + 0x10) : 0;
    uint32_t rec = env->a520(ctx[4]);
    if (unit) {
        const uint8_t on = *(uint8_t*)(uintptr_t)(rec + 0xC4 + mode);
        const uint32_t f = *(uint32_t*)(unit + 0xC4);
        *(uint32_t*)(unit + 0xC4) = on ? (f | 2u) : (f & 0xFFFFFFFDu);
    }
    rec = env->a520(ctx[4]);
    if (*(uint8_t*)(uintptr_t)(rec + 0x141)) {
        const uint32_t gameA8 = *(const uint32_t*)(uintptr_t)(ctx[0] + 0xA8);
        rec = env->a520(ctx[4]);
        env->ea80(ctx[0], ctx[1], 1, (*(uint32_t*)(uintptr_t)(rec + 0xDC) >> 8) + 1 + gameA8, 0, 0, 0);
    }
    return 1;
}
