// Capture probe: argument layout is r[0..3] = descriptor arguments in order, result r[4].
#include <emscripten.h>
#include "hookenv.h"
static uint32_t body(uint32_t* r, bool gate) {
    HookEnv e((uint32_t*)(uintptr_t)r[2], (const uint32_t*)(uintptr_t)r[3]);
    const uint8_t* s = (const uint8_t*)(uintptr_t)r[0];
    uint32_t nd = r[1];
    if (gate && s[0xc] != 5 && s[9] != 0) return 1;
    uint32_t a4, a10; memcpy(&a4, s + 4, 4); memcpy(&a10, s + 0x10, 4);
    if (!e.h(0x33F30, a4, a10, 0, 0, 0)) return 0;
    uint32_t cb; memcpy(&cb, (const uint8_t*)(uintptr_t)nd + 0xf0, 4);
    return e.h(HookEnv::handlerRva(cb), 0, nd, 0, 0, 0) & 0xFF;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_c1ff0(uint32_t* r) { r[4] = body(r, false); }
