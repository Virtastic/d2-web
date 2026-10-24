// 1.13c D2Game+0x33740 probe, FUNCTION mode, a retail recording: the rule
// replayed with the three callees as recorders (log entry format: src/core/fn/hookenv.h). r[0] = pOp (ecx),
// r[1] = sc (0xA4A2 -> the level id), r[2] = the log blob.
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_33740(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[2], (const uint32_t*)(uintptr_t)r[1]);
    const uint32_t op = r[0], game = e.rd32(op), unit = e.rd32(op + 4), room = e.rd32(op + 8);
    const uint32_t frame = e.rd32(game + 0xA8);
    if (e.h(0xA4A2, 0, 0, 0, 0, 0, room) == 1) {
        e.h(0x8EA80, 0, game, unit, 0, 0, frame + 0x3C, 0, 0, 0);
    } else {
        e.h(0xA556, 0, 0, 0, 0, 0, unit, 2);
        e.h(0x8EA80, 0, game, unit, 0, 0, frame + 0x19, 0, 0, 0);
    }
    r[4] = 0;
}
