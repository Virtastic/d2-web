// Capture probe: argument layout is r[0..3] = descriptor arguments in order, result r[4].
#include <emscripten.h>
#include "hookenv.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_b6d30(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[2], (const uint32_t*)(uintptr_t)r[3]);
    const uint32_t u1 = r[0], u2 = r[1];
    r[4] = 0;
    if (*(const uint8_t*)(uintptr_t)(u1 + 0x18) != *(const uint8_t*)(uintptr_t)(u2 + 0x18)) return;
    const uint32_t y = e.h(0x1130, 0, u1), x = e.h(0x1100, 0, u1);
    r[4] = e.h(0xB08F0, u2, 0, 0, 0, 0, x, y) < 0x3FC;
}
