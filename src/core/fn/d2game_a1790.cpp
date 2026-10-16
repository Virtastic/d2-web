// 1.13c D2Game+0xA1790: regs ebx = pUnit; r[1] = arg; r[2] = sc, r[3] = log.
#include <emscripten.h>
#include "hookenv.h"
#include "../d2game/skill_helpers_a19a0.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_a1790(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[3], (const uint32_t*)(uintptr_t)r[2]);
    r[4] = a19a0::HandItemUse(e, r[0], r[1]);
}
