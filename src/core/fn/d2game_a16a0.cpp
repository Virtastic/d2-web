// 1.13c D2Game+0xA16A0: regs esi = pUnit; stack a0, a1 = r[1], r[2]; r[3] = sc, r[5] = log.
#include <emscripten.h>
#include "hookenv.h"
#include "../d2game/skill_helpers_a19a0.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_a16a0(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[5], (const uint32_t*)(uintptr_t)r[3]);
    r[4] = a19a0::SpendHandUse(e, r[0], r[1], r[2]);
}
