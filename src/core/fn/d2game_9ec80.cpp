// 1.13c D2Game+0x9EC80: regs ebx = pUnit; r[1] = sc, r[2] = hook log.
#include <emscripten.h>
#include "hookenv.h"
#include "../d2game/skill_helpers_a19a0.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_9ec80(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[2], (const uint32_t*)(uintptr_t)r[1]);
    r[4] = a19a0::HasManaForUsedSkill(e, r[0]);
}
