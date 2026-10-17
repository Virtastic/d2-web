// 1.13c D2Game+0x6F930: regs ecx = pGame, edx = request; r[5] = sc, r[6] = log, r[7] = sl: owner unit (+0), sgptDataTables slot (+4), gdwBitMasks slot (+8), collision table (+0x10).
#include <emscripten.h>
#include "hookenv.h"
#include "../d2game/missile_create_6f930.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_6f930(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[6], (const uint32_t*)(uintptr_t)r[5]); e.dtPtr = r[7] + 4;
    HookEnv::wr32(r[1] + 0x54, 0xB2000000u | 0x79100);   // the capture pins the init callback (gmodptr) after recording IN
    r[4] = m6f930::Create(e, r[0], r[1], HookEnv::rd32(r[7] + 8), r[7] + 0x10);
}
