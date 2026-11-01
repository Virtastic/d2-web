// D2Game+0x26E40: the whirlwind/frenzy hit scheduler. ESI = unit, stack: game, skill (ret 8). Returns 0, 1 or 2.
// A monster rolls its seed (unit +0x20/+0x24, the 0x6AC690C5 LCG) and returns 1 on an even roll. Otherwise, in a
// classic game (game +0x70 zero) it returns 1. The skill's next-hit frame (param 4, 10631 via 0xAB44) unset: set it
// to the game frame (+0xA8) + 4 and return 1; still in the future: return 0. Else the hit's spacing comes from the
// weapon's attack speed (0x372C0 finds the weapon, 0xB336 reads the speed; no weapon: 10) in the table below; two
// matching weapons (0x26A30) return 2. The next frame becomes param 4 + spacing, or frame + spacing when that is
// already past.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game26E40Env {
    int32_t (*skillParam4)(uint32_t skill);                          // thunk 0xAB44 (10631)
    void (*setSkillParam4)(uint32_t skill, int32_t v);               // thunk 0xAB20 (11031)
    uint32_t (*findWeapon)(uint32_t unit);                           // D2Game 0x372C0 (stdcall 1)
    int32_t (*attackSpeed)(uint32_t unit, uint32_t weapon);          // thunk 0xB336
    uint32_t (*dualWield)(uint32_t unit);                            // D2Game 0x26A30 (EAX unit)
};

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }

// 1.13c D2Game+0x26E40
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2game_26e40(uint32_t unit, uint32_t game, uint32_t skill, const D2Game26E40Env* env)
{
    if (unit && rd32(unit, 0) == 1) {
        const uint64_t s = (uint64_t)rd32(unit, 0x20) * 0x6ac690c5u + rd32(unit, 0x24);
        const uint32_t lo = (uint32_t)s, hi = (uint32_t)(s >> 32);
        std::memcpy((void*)(uintptr_t)(unit + 0x20), &lo, 4);
        std::memcpy((void*)(uintptr_t)(unit + 0x24), &hi, 4);
        return (lo & 1) == 0;
    }
    if (!rd32(game, 0x70)) return 1;
    int32_t next = env->skillParam4(skill);
    if (!next) {
        env->setSkillParam4(skill, (int32_t)(rd32(game, 0xa8) + 4));
        return 1;
    }
    if ((int32_t)rd32(game, 0xa8) < next) return 0;
    uint32_t ret = 1;
    int32_t gap;
    const uint32_t weapon = env->findWeapon(unit);
    if (!weapon) gap = 10;
    else {
        const int32_t sp = env->attackSpeed(unit, weapon);
        gap = sp < 12 ? 4 : sp < 15 ? 6 : sp < 18 ? 8 : sp < 20 ? 10 : sp < 23 ? 12 : (sp >= 26 ? 16 : 14);
    }
    if (env->dualWield(unit)) ret = 2;
    const int32_t frame = (int32_t)rd32(game, 0xa8);
    next = (int32_t)((uint32_t)next + (uint32_t)gap);
    if (next < frame) next = (int32_t)((uint32_t)frame + (uint32_t)gap);
    env->setSkillParam4(skill, next);
    return ret;
}
