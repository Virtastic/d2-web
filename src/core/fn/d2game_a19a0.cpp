// 1.13c D2Game+0xA19A0: regs eax = pUnit; stack game, id, level = r[1..3], a5, a6, a7 = r[5..7]; r[8] = sc, r[9] = log,
// r[10] = g (sgptDataTables slot at D2Game+0xF829C, gdwBitMasks slot at g+4 = D2Game+0xF8BE0), r[11] = tt (word tables at D2Game+0x107338).
#include <emscripten.h>
#include "hookenv.h"
#include "../d2game/skill_helpers_a19a0.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_a19a0(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[9], (const uint32_t*)(uintptr_t)r[8]);
    e.dtPtr = r[10]; e.bmSlot = r[10] + 4; e.tbl = r[11]; e.fileStr = 0xB20FB980;
    for (int i = 3; i < 21; i += (i == 3 ? 6 : 11)) HookEnv::wr32(r[11] + 0x170 + i * 4, 0xB2000000u | 0x78710);   // the capture pins these (gmodptr) after recording IN
    r[4] = a19a0::ExecuteSkill(e, r[0], r[1], r[2], r[3], r[5], r[6], r[7]);
}
