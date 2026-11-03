// D2Game+0x27B90: a leap-attack step on the unit's path target. ECX = game, EDX = unit, stack: a, b (ret 8).
// After the unit check (0xE07A0) the path's target unit (10392 on +0x2C) must exist and not be the unit. With
// bit 8 of the unit's +0x38 flags clear the strike runs at once: 0x273C0 (game, unit, a, b), then the durability
// drain 0xDED30 (game, unit, target), returning 1. With it set, the unit's +0xC4 flags gain 0x40 and an area search
// (0xA0C70, range level + 4) fills the block {0, -1, 0, id, id, 0} (id = the target's +0x0C); word 0, else word 2,
// becomes the target, which the unit is pointed at (11146 via 0xACD0) before the same strike. No target: 0.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#include "d2game_27b90_env.h"

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }

// 1.13c D2Game+0x27B90
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2game_27b90(uint32_t game, uint32_t unit, uint32_t a, uint32_t b, const D2Game27B90Env* env)
{
    env->checkUnit(unit, game);
    uint32_t t = env->pathTargetUnit(rd32(unit, 0x2c));
    if (t == unit || !t) return 0;
    if (rd32(unit, 0x38) & 0x100) {
        const uint32_t f = rd32(unit, 0xc4) | 0x40;
        std::memcpy((void*)(uintptr_t)(unit + 0xc4), &f, 4);
        const int32_t range = env->unitLevel(unit) + 4;
        const uint32_t id = rd32(t, 0xc);
        uint32_t blk[6] = { 0, 0xffffffffu, 0, id, id, 0 };
        env->areaSearch(game, blk, unit, 0, 0, range, 0x2a783, env->callback);
        t = blk[0] ? blk[0] : blk[2];
        if (!t) return 0;
        env->faceTarget(unit, t);
    }
    env->strike(game, unit, a, b);
    env->drainDurability(game, unit, t);
    return 1;
}
