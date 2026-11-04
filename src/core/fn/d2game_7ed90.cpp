// d2game+0x7ED90 probe: r[0] = ECX game, r[1] = EDX unit, r[2] = stack task. Blobs lg 0, sc 1, game 2, unit 3, md 4, ai 5, sub 6, task 7, M 8, row 9,
// tgt 10, tmd 11, tai 12, mrec 13, gm 14 (the pinned D2Game+0xF8BE0 global), own 15.
#include "d2game_77d_probe.h"
extern "C" void d2game_7ed90_port(const D2Game77dEnv* env, uint32_t game, uint32_t unit, uint32_t task);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_7ed90(uint32_t* r) {
    uint32_t* lg = (uint32_t*)(uintptr_t)r[14];
    HookEnv e(lg, (const uint32_t*)(uintptr_t)r[16]); D77::E = &e; D77::GM = r[14 + 2 * 14];
    d2game_7ed90_port(&D77::env(), r[0], r[1], r[2]);
    r[4] = 0;
    d2TagLog(lg, r, 16);
}
