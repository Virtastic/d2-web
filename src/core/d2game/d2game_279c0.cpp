// D2Game+0x279C0: the monster leap's landing strike. EAX = target, ECX = unit, EBX = skill, stack: game (ret 4).
// With a target and a skill: mode 4 on the unit (0x77240), the attack follow-up (0x77790, ESI = unit),
// item durability drain (0xDED30: game, unit, target), then the skill remembers the target's type
// (D2Common 10748 via 0xAD0C) and id (+0x0C, D2Common 11031 via 0xAB20).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game279C0Env {
    void (*setMode)(uint32_t unit, uint32_t mode);                       // D2Game 0x77240 (stdcall 2)
    void (*attackFollowUp)(uint32_t unit, uint32_t game);                // D2Game 0x77790 (ESI unit, stack game, ret 4)
    void (*drainDurability)(uint32_t game, uint32_t unit, uint32_t target);  // D2Game 0xDED30 (stdcall 3)
    void (*setParam3)(uint32_t skill, uint32_t v);                       // thunk 0xAD0C
    void (*setParam4)(uint32_t skill, uint32_t v);                       // thunk 0xAB20
};

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }

// 1.13c D2Game+0x279C0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2game_279c0(uint32_t target, uint32_t unit, uint32_t skill, uint32_t game,
                                                      const D2Game279C0Env* env)
{
    if (!target || !skill) return 0;
    env->setMode(unit, 4);
    env->attackFollowUp(unit, game);
    env->drainDurability(game, unit, target);
    env->setParam3(skill, rd32(target, 0));
    env->setParam4(skill, rd32(target, 0xc));
    return 1;
}
