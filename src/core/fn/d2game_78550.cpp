// 1.13c D2Game+0x78550: regs ecx = pGame, edx; stack a, idx, b, c, noCheck = r[2], r[3], r[5..7]; r[8] = sc, r[9] = log, r[10] = handler table.
#include <emscripten.h>
#include "hookenv.h"
#include "../d2game/skill_helpers_a19a0.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_78550(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[9], (const uint32_t*)(uintptr_t)r[8]); e.tbl = r[10];
    for (int i = 0; i < 4; i++) HookEnv::wr32(r[10] + i * 8, 0xB2000000u | 0x78710);   // the capture pins these after recording IN
    a19a0::Dispatch(e, r[0], r[1], r[2], r[3], r[5], r[6], r[7]);
    r[4] = 0;
}
