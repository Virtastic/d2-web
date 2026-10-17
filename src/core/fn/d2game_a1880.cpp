// 1.13c D2Game+0xA1880: regs eax = X, esi = pUnit; stack game, a1, a2, a3, dx, dy, y, check = r[2], r[3], r[5..10]; r[11] = sc, r[12] = log.
#include <emscripten.h>
#include "hookenv.h"
#include "../d2game/skill_helpers_a19a0.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_a1880(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[12], (const uint32_t*)(uintptr_t)r[11]);
    r[4] = a19a0::BuildSkillRequestA(e, r[0], r[1], r[2], r[3], r[5], r[6], r[7], r[8], r[9], r[10]);
}
