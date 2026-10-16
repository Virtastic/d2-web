// 1.13c D2Game+0x9F850: regs eax = skillId, ecx = pUnit; stack a, level = r[2], r[3]; r[5] = sc, r[6] = log, r[7] = sgptDataTables slot.
#include <emscripten.h>
#include "hookenv.h"
#include "../d2game/skill_helpers_a19a0.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_9f850(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[6], (const uint32_t*)(uintptr_t)r[5]); e.dtPtr = r[7]; e.fileStr = 0xB20FB980;
    r[4] = a19a0::PaySkillCost(e, r[0], r[1], r[2], r[3]);
}
