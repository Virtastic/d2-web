// Capture probe: argument layout is r[0..3] = descriptor arguments in order, result r[4].
#include <emscripten.h>
#include "hookenv.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_b6c90(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[3], (const uint32_t*)(uintptr_t)r[2]);
    const uint32_t u = r[0], d = r[1];
    r[4] = 0;
    if (!u) return;
    const int32_t k = (int32_t)e.h(0xA49C, 0, 0, 0, 0, 0, u, 0xB20FB434u, 0x131);
    if (k == 0x3b || k == 0x38) { r[4] = 1; return; }
    if (k == 0x33) { r[4] = (int32_t)e.h(0xA400, 0, 0, 0, 0, 0, d, u, 1) > 3; return; }
    if (k == 0x1b) r[4] = (int32_t)e.h(0xA400, 0, 0, 0, 0, 0, d, u, 1) > 7;
}
