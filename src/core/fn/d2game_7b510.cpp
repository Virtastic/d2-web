// Capture probe: argument layout is r[0..3] = descriptor arguments in order, result r[4].
#include <emscripten.h>
#include <cstring>
#include "hookenv.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_7b510(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[3], (const uint32_t*)(uintptr_t)r[2]);
    const uint32_t g = r[0], t = r[1];
    const uint32_t v = e.h(0xA478, 0, 0, 0, 0, 0, g);
    const uint32_t idx = e.h(0xA556, 0, 0, 0, 0, 0, v);
    uint32_t st; memcpy(&st, (const void*)(uintptr_t)(t + idx * 4 + 0xf0), 4);
    int32_t* f = (int32_t*)(uintptr_t)(st + 0x2d4);
    if (*f < 0) {
        if ((int32_t)idx < 0x28) *f = 0;
        else { *f = (int32_t)e.h(0x1780, 3, g + 0x20); e.j(0x1780, 0); }
    }
    r[4] = (uint32_t)*f;
}
