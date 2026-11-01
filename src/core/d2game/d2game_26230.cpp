// D2Game+0x26230: the unique-monster corpse poison cloud (MONSTERUNIQUE_CastCorpsePoisonCloud).
// ECX = game, EDX = unit, two unused stack words (ret 8). Missile 0x9B at the unit's position, its
// count the unit's stat 0x0C (at least 1). The position: players, missiles and items (types 2, 4, 5)
// read the static path (+0x2C) directly (+0x0C x, +0x10 y); others ask D2Common for the dynamic path's.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game26230Env {
    int32_t (*getStat)(uint32_t unit, uint32_t stat, uint32_t layer);   // D2Common 10973 (thunk 0xA42A)
    int32_t (*pathY)(uint32_t path);                                    // D2Common 10750 (thunk 0xA3B8)
    int32_t (*pathX)(uint32_t path);                                    // D2Common 10867 (thunk 0xA454)
    uint32_t (*spawnMissile)(uint32_t game, uint32_t id, uint32_t unit, uint32_t a3, int32_t count, uint32_t a5,
                             uint32_t a6, int32_t x, int32_t y, uint32_t a9);   // D2Game 0xA28F0 (ECX game, EDX id, 8 stack)
};

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static inline bool staticPath(uint32_t t) { return t == 2 || (t > 3 && t < 6); }

// 1.13c D2Game+0x26230
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_26230(uint32_t game, uint32_t unit, const D2Game26230Env* env)
{
    int32_t count = env->getStat(unit, 0xc, 0);
    if (count < 2) count = 1;
    const uint32_t path = rd32(unit, 0x2c);
    const int32_t y = staticPath(rd32(unit, 0)) ? (int32_t)rd32(path, 0x10) : path ? env->pathY(path) : 0;
    const int32_t x = staticPath(rd32(unit, 0)) ? (int32_t)rd32(path, 0xc) : path ? env->pathX(path) : 0;
    env->spawnMissile(game, 0x9b, unit, 0, count, 0, 0, x, y, 1);
}
