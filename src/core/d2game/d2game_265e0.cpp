// D2Game+0x265E0: the unique-monster corpse explosion. ECX = game, EDX = unit, two unused stack words (ret 8).
// Missile 0x75 at the unit's position (x first, then y; types 2, 4, 5 read the static path directly), then, if
// it spawned, the area damage (0xDE380) at that position with a 0x1C-dword damage block that is zero except
// words 2 and 4 (0x6400 each), radius 6, flags 0x583.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game265E0Env {
    int32_t (*pathX)(uint32_t path);                                    // D2Common 10867 (thunk 0xA454)
    int32_t (*pathY)(uint32_t path);                                    // D2Common 10750 (thunk 0xA3B8)
    uint32_t (*spawnMissile)(uint32_t game, uint32_t id, uint32_t unit, uint32_t a3, int32_t count, uint32_t a5,
                             uint32_t a6, int32_t x, int32_t y, uint32_t a9);   // D2Game 0xA28F0
    void (*areaDamage)(uint32_t game, uint32_t missile, int32_t x, int32_t y, int32_t radius, const uint32_t* dmg,
                       uint32_t a6, uint32_t a7, uint32_t cb, uint32_t flags);  // D2Game 0xDE380 (stdcall 10)
};

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static inline bool staticPath(uint32_t t) { return t == 2 || (t > 3 && t < 6); }

// 1.13c D2Game+0x265E0
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_265e0(uint32_t game, uint32_t unit, const D2Game265E0Env* env)
{
    const uint32_t path = rd32(unit, 0x2c);
    const int32_t x = staticPath(rd32(unit, 0)) ? (int32_t)rd32(path, 0xc) : path ? env->pathX(path) : 0;
    const int32_t y = staticPath(rd32(unit, 0)) ? (int32_t)rd32(path, 0x10) : path ? env->pathY(path) : 0;
    const uint32_t missile = env->spawnMissile(game, 0x75, unit, 0, 1, 0, 0, x, y, 1);
    if (!missile) return;
    uint32_t dmg[0x1c] = {};
    dmg[2] = 0x6400;
    dmg[4] = 0x6400;
    env->areaDamage(game, missile, x, y, 6, dmg, 0, 0, 0, 0x583);
}
