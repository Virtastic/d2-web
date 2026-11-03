// 1.13c D2Game+0x698E0 probe: the port over the HookEnv recorder.
// Descriptor `regs eax edi`, args 4: r[0] unit, r[1] edi, r[2] hook log, r[3] hook values, r[4] result.
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"

struct D2Game698E0Env {
    void     (*fatal)(uint32_t line);
    uint32_t (*first)(uint32_t inv);
    uint32_t (*data)(uint32_t item);
    uint32_t (*test)(uint32_t data, uint32_t edi);
    uint32_t (*next)(uint32_t item);
};
uint32_t D2Game_698E0(uint32_t unit, uint32_t edi, const D2Game698E0Env& e);

namespace {
HookEnv* H;
void fatal(uint32_t) { abort(); }
uint32_t first(uint32_t inv) { return H->h(0xA3DC, 0, 0, 0, 0, 0, inv); }
uint32_t data(uint32_t it) { return H->h(0xA46C, 0, 0, 0, 0, 0, it); }
uint32_t test(uint32_t d, uint32_t edi) { return H->h(0xA430, 0, 0, 0, 0, 0, d, edi); }
uint32_t next(uint32_t it) { return H->h(0xA4CC, 0, 0, 0, 0, 0, it); }
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_698e0(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[2], (const uint32_t*)(uintptr_t)r[3]);
    H = &e;
    const D2Game698E0Env env = { fatal, first, data, test, next };
    r[4] = D2Game_698E0(r[0], r[1], env);
}
