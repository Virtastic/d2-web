// 1.13c D2Game+0x33620 probe, FUNCTION mode, a retail recording:
// the row's rule against the retail vector. ecx = pOp (r[0]), r[1] = log blob. Logged as the descriptor's hooks record.
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_33620(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[1], nullptr);
    const uint32_t op = r[0], game = HookEnv::rd32(op), obj = HookEnv::rd32(op + 4), txt = HookEnv::rd32(op + 0x10);
    r[4] = 0;   // void
    if (obj && HookEnv::rd32(obj + 0x10) != 0) return;
    const uint32_t frame = HookEnv::rd32(game + 0xa8);
    e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 1);
    e.h(0x8EA80, 0, game, obj, 0, 0, frame + 27, 0, 0, 0);
    e.h(0x8EA80, 0, game, obj, 0, 0, (HookEnv::rd32(txt + 0xdc) >> 8) + frame + 1, 0, 0, 0);
}
