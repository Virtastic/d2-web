// 1.13c D2Game+0x9FA30: regs eax = pUnit, ebx = pX, ecx = this; r[3] = pY; r[5] = sc, r[6] = log.
#include <emscripten.h>
#include "hookenv.h"
#include "../d2game/skill_helpers_a19a0.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_9fa30(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[6], (const uint32_t*)(uintptr_t)r[5]);
    r[4] = a19a0::GetTargetCoords(e, r[0], r[1], r[3], r[2]);
}
