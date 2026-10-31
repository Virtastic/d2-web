// D2Game+0x4E230 -- one fifth of a non-positive resist-group value once the unit's stat reaches 100.
// edi = stat, ebx = value, esi = unit. Only stats 0x24 0x25 0x27 0x29 0x2b 0x2d take the rule; every other stat
// returns value. The rule: value <= 0, the unit is not a player (null counts as not a player), #a7f6(unit) == 0 and
// #a3e2(unit, stat, 0) >= 100 -> value / 5 (signed).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game4E230Env {
    uint32_t (*a7f6)(uint32_t unit);
    int32_t  (*a3e2)(uint32_t unit, uint32_t stat, uint32_t layer);
};

// 1.13c D2Game+0x4E230
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_4e230(uint32_t stat, int32_t value, uint32_t unit, const D2Game4E230Env* env)
{
    switch (stat) {
    case 0x24: case 0x25: case 0x27: case 0x29: case 0x2b: case 0x2d: break;
    default: return value;
    }
    if (value > 0) return value;
    if (unit != 0) { uint32_t t; std::memcpy(&t, (const void*)(uintptr_t)unit, 4); if (t == 0) return value; }
    if (env->a7f6(unit) != 0) return value;
    if (env->a3e2(unit, stat, 0) < 100) return value;
    return value / 5;
}
