// 1.13c D2Game+0xA6AB0 (EAX other, ECX skill id, ESI target, EDI unit; stack game, flag, x, y; ret 0x10)
#include "d2game_a6ab0.h"
#include <cstring>

static uint32_t rd(const void* p, uint32_t off) { uint32_t v; memcpy(&v, (const uint8_t*)p + off, 4); return v; }

uint32_t d2game_a6ab0(const D2GameA6AB0Env& env, const void* other, uint32_t skillId, uint8_t* target, void* unit,
                      uint32_t game, uint32_t flag, uint32_t x, uint32_t y)
{
    if (target && (target == other || target == unit)) return 0;
    void* skill = env.getSkill(unit, skillId, -1);
    if (!skill) return 0;
    if (target) {
        if (!(rd(target, 0xc4) >> 2 & 1)) return 0;
        if (env.roomBlocked(env.getRoom(unit))) return 0;
    }
    if (env.skillMode(unit, skill) == 1 && flag == 0)
        return env.setMode(game, unit, target, 0xf, 0, 0, 1, 4);
    const uint8_t level = (uint8_t)env.skillLevel(skill);
    return env.doSkill(unit, game, level, skillId, target, x, y);
}
