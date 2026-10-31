// D2Game+0x3D1E0 -- periodic spawn of a unit's scatter missile/event at a seeded offset.
// eax = period, ebx = unit, stack (game, radius, a3, a4) ret 0x10; returns 0 or 0x6F930's result.
// period == 0, #b28e(unit) % period != 0 (signed), unit == 0 or unit+0xc8 bit 10 clear -> 0.
// t = 0xE03A0(ecx game, edx unit[0x98], eax unit[0x94]); t == 0 or #a478(unit) == 0 -> 0.
// #a628(ecx unit+0x20, edx #b28e(unit) + 0x1100(unit)) reseeds; a zeroed 0x5c block gets +0 = 3, +4 = t,
// +0x2c = #b26a(unit), +0x30 = #b0fc(unit), +0x10 = a3, +0x14 = x, +0x18 = y where
// x = 0x1100(unit) + roll(2(r-1)) - (r-1), y = 0x1130(unit) + roll(2(r-1)) - (r-1).
// #afe8(#a478(unit), x, y, a4) non-zero (16 bits) -> 0, else 0x6F930(ecx game, edx &block).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game3D1E0Env {
    int32_t  (*b28e)(uint32_t unit);
    uint32_t (*subE03A0)(uint32_t ecx, uint32_t edx, uint32_t eax);
    uint32_t (*a478)(uint32_t unit);
    int32_t  (*sub1100)(uint32_t unit);
    int32_t  (*sub1130)(uint32_t unit);
    void     (*a628)(uint32_t seed, uint32_t value);
    uint32_t (*b26a)(uint32_t unit);
    uint32_t (*b0fc)(uint32_t unit);
    int32_t  (*roll)(uint32_t seed, int32_t range);        // 0x11D0 (ecx seed, esi range)
    uint16_t (*afe8)(uint32_t room, int32_t x, int32_t y, uint32_t a4);
    uint32_t (*sub6F930)(uint32_t game, uint32_t block);
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

// 1.13c D2Game+0x3D1E0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_3d1e0(int32_t period, uint32_t unit, uint32_t game, int32_t radius,
                                                           uint32_t a3, uint32_t a4, const D2Game3D1E0Env* env)
{
    if (period == 0) return 0;
    if (env->b28e(unit) % period != 0) return 0;
    if (unit == 0 || ((rd32(unit + 0xc8) >> 10) & 1) == 0) return 0;
    const uint32_t t = env->subE03A0(game, rd32(unit + 0x98), rd32(unit + 0x94));
    if (t == 0 || env->a478(unit) == 0) return 0;
    const uint32_t seed = unit + 0x20;
    env->a628(seed, (uint32_t)(env->b28e(unit) + env->sub1100(unit)));
    uint32_t block[0x17] = {};
    block[0] = 3;
    block[1] = t;
    block[0x2c / 4] = env->b26a(unit);
    block[0x30 / 4] = env->b0fc(unit);
    block[0x10 / 4] = a3;
    const int32_t r = radius - 1;
    const int32_t dx = env->roll(seed, r * 2) - r;
    const int32_t dy = env->roll(seed, r * 2) - r;
    const int32_t x = env->sub1100(unit) + dx;
    block[0x14 / 4] = (uint32_t)x;
    const int32_t y = env->sub1130(unit) + dy;
    block[0x18 / 4] = (uint32_t)y;
    if (env->afe8(env->a478(unit), x, y, a4) != 0) return 0;
    return env->sub6F930(game, (uint32_t)(uintptr_t)block);
}
