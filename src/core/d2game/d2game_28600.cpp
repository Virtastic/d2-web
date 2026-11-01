// D2Game+0x28600: the leap-attack skill's start (SKILLS_SrvSt41_LeapAttack). ECX = game, EDX = unit, stack
// skillId and one unused word (ret 8); returns 1 when the unit starts the leap or strikes its target at once.
// Its skill (D2Common 10630) missing returns 0. The path's target unit (10392; the unit itself counts as none)
// and a landing spot (10566, writing x into the skill-id stack slot and y into a local) decide: no spot means a
// target in melee range (11138) is struck now (D2Game 0xA2DE0); a spot needs a room (10331), and the unit then
// moves there (10565 with flag 0x80), its mode is set (0xE2240, 1), the skill records x/y, the target type and
// id (or type 6 with no target), flags 0x1080, stat 0x12 = 1; a target with state 0x36 is hit by 0xA2F90 (100).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game28600Env {
    uint32_t (*getSkill)(uint32_t unit, uint32_t skillId);                       // D2Common 10630 (0xA3F4, ECX/EDX)
    void (*checkUnit)(uint32_t unit, uint32_t game);                             // D2Game 0xE07A0 (EAX unit, ECX game)
    uint32_t (*pathTarget)(uint32_t path);                                       // D2Common 10392 (0xA598)
    uint32_t (*landing)(uint32_t unit, uint32_t target, int32_t* x, int32_t* y); // D2Common 10566 (0xB33C)
    uint32_t (*inMelee)(uint32_t unit, uint32_t target, uint32_t a);             // D2Common 11138 (0xA754)
    void (*strikeNow)(uint32_t unit, uint32_t target);                           // D2Game 0xA2DE0 (EAX unit, 1 stack)
    uint32_t (*room)(uint32_t unit);                                             // D2Common 10331 (0xA478)
    uint32_t (*sizeOf)(uint32_t unit);                                           // D2Common 10123 (0xAD00)
    void (*moveTo)(uint32_t room, int32_t x, int32_t y, uint32_t v, uint32_t flags);   // D2Common 10565 (0xAD2A)
    void (*setMode)(uint32_t unit, uint32_t mode);                               // D2Game 0xE2240 (ECX/EDX)
    void (*skillX)(uint32_t skill, int32_t x);                                   // D2Common 10243 (0xAC94)
    void (*skillY)(uint32_t skill, int32_t y);                                   // D2Common 10554 (0xACDC)
    void (*skillTargetType)(uint32_t skill, uint32_t type);                      // D2Common 10748 (0xAD0C)
    void (*skillTargetId)(uint32_t skill, uint32_t id);                          // D2Common 11031 (0xAB20)
    void (*skillFlags)(uint32_t skill, uint32_t flags);                          // D2Common 10137 (0xA78A)
    void (*setStat)(uint32_t unit, uint32_t stat, uint32_t v);                   // D2Common 10945 (0xA3CA)
    uint32_t (*hasState)(uint32_t unit, uint32_t state);                         // D2Common 10494 (0xA68E)
    void (*stateHit)(uint32_t target, uint32_t pct, uint32_t game);              // D2Game 0xA2F90 (ECX target, EAX pct, 1 stack)
};

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }

// 1.13c D2Game+0x28600
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2game_28600(uint32_t game, uint32_t unit, uint32_t skillId, uint32_t a1, const D2Game28600Env* env)
{
    (void)a1;
    const uint32_t skill = env->getSkill(unit, skillId);
    if (!skill) return 0;
    env->checkUnit(unit, game);
    uint32_t t = env->pathTarget(rd32(unit, 0x2c));
    if (t == unit) t = 0;
    int32_t x = (int32_t)skillId, y = 0;
    if (!env->landing(unit, t, &x, &y)) {
        if (t && env->inMelee(unit, t, 0)) { env->strikeNow(unit, t); return 1; }
        return 0;
    }
    const uint32_t room = env->room(unit);
    if (!room) return 0;
    env->moveTo(room, x, y, env->sizeOf(unit), 0x80);
    env->setMode(unit, 1);
    env->skillX(skill, x);
    env->skillY(skill, y);
    if (t) { env->skillTargetType(skill, rd32(t, 0)); env->skillTargetId(skill, rd32(t, 0xc)); }
    else env->skillTargetType(skill, 6);
    env->skillFlags(skill, 0x1080);
    env->setStat(unit, 0x12, 1);
    if (t && env->hasState(t, 0x36)) env->stateHit(t, 0x64, game);
    return 1;
}
