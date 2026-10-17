// 1.13c D2Game+0xA28F0: regs ecx = pGame, edx = a0; stack u, a1, a2, dx, dy, px, py, check = r[2], r[3], r[5..10]; r[11] = sc, r[12] = log.
#include <emscripten.h>
#include "hookenv.h"
#include "../d2game/skill_helpers_a19a0.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_a28f0(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[12], (const uint32_t*)(uintptr_t)r[11]);
    r[4] = a19a0::BuildSkillRequestB(e, r[0], r[1], r[2], r[3], r[5], r[6], r[7], r[8], r[9], r[10]);
}
