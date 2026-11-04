// d2game+0x83FE0 probe: r[0] = ECX game, r[1] = EDX unit, r[2] = stack task. Blobs lg 0, sc 1, game 2, unit 3, task 4, M 5, row 6, f 7, res 8, tgt 9.
#include "d2game_77d_probe.h"
extern "C" void d2game_83fe0_port(const D2Game77dEnv* env, uint32_t game, uint32_t unit, uint32_t task);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_83fe0(uint32_t* r) {
    uint32_t* lg = (uint32_t*)(uintptr_t)r[14];
    HookEnv e(lg, (const uint32_t*)(uintptr_t)r[16]); D77::E = &e;
    d2game_83fe0_port(&D77::env(), r[0], r[1], r[2]);
    r[4] = 0;
    d2TagLog(lg, r, 10);
}
