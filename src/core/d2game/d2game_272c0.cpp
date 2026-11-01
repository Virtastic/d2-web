// D2Game+0x272C0: finds the leap-attack target (SKILLS_FindLeapAttackTarget). EDI = unit, stack: game, skill (ret 8).
// No skill: 0. After the unit check (0xE07A0), the path's target unit (10392 on +0x2C) is taken when it is not the
// unit itself and passes 11138 (unit, target, 0). Otherwise the skill's remembered target (param3 type, param4 id)
// is looked up (0xE03A0; type 6 means none). Failing both, an area search (0xA0C70) around the unit fills a local
// six-dword block {0,-1,0,-1,-1,0}: word 0, else word 2, is the target. With no target the skill's type is set to 6
// and 0 returned; with one, the skill remembers its type (+0) and id (+0x0C) and the target is returned.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game272C0Env {
    void (*checkUnit)(uint32_t unit, uint32_t game);                               // D2Game 0xE07A0 (EAX unit, ECX game)
    uint32_t (*pathTargetUnit)(uint32_t path);                                     // thunk 0xA598
    uint32_t (*canTarget)(uint32_t unit, uint32_t target, uint32_t v);             // thunk 0xA754
    int32_t (*skillParam3)(uint32_t skill);                                        // thunk 0xAD3C
    int32_t (*skillParam4)(uint32_t skill);                                        // thunk 0xAB44
    uint32_t (*findUnit)(int32_t type, uint32_t game, int32_t id);                 // D2Game 0xE03A0 (EAX type, ECX game, EDX id)
    int32_t (*unitLevel)(uint32_t unit);                                           // thunk 0xA77E
    void (*areaSearch)(uint32_t game, uint32_t* blk, uint32_t unit, uint32_t a1, uint32_t a2, int32_t range,
                       uint32_t flags, uint32_t callback);                         // D2Game 0xA0C70 (EAX game, ECX blk, 8 stack)
    void (*setParam3)(uint32_t skill, uint32_t v);                                 // thunk 0xAD0C
    void (*setParam4)(uint32_t skill, uint32_t v);                                 // thunk 0xAB20
    uint32_t callback;                                                             // D2Game 0x9ED70
};

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }

// 1.13c D2Game+0x272C0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2game_272c0(uint32_t unit, uint32_t game, uint32_t skill, const D2Game272C0Env* env)
{
    if (!skill) return 0;
    env->checkUnit(unit, game);
    uint32_t t = env->pathTargetUnit(rd32(unit, 0x2c));
    if (t == unit) t = 0;
    else if (t && !env->canTarget(unit, t, 0)) t = 0;
    if (!t) {
        const int32_t type = env->skillParam3(skill);
        const int32_t id = env->skillParam4(skill);
        if (type != 6) t = env->findUnit(type, game, id);
        if (!t) {
            uint32_t blk[6] = { 0, 0xffffffffu, 0, 0xffffffffu, 0xffffffffu, 0 };
            env->areaSearch(game, blk, unit, 0, 0, env->unitLevel(unit) + 4, 0x2a783, env->callback);
            t = blk[0] ? blk[0] : blk[2];
            if (!t) { env->setParam3(skill, 6); return 0; }
        }
    }
    env->setParam3(skill, rd32(t, 0));
    env->setParam4(skill, rd32(t, 0xc));
    return t;
}
