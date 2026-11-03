// 1.13c D2Game+0x687D0 probe: the port over the HookEnv recorder.
// Descriptor `regs ecx`, args 3: r[0] quest record, r[1] hook log, r[2] hook values, r[4] result (unused).
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"

struct D2Game687D0Env {
    uint32_t (*cb)(uint32_t rva);
    uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t line, uint32_t zero);
};
void D2Game_687D0(uint8_t* q, const D2Game687D0Env& e);

namespace {
HookEnv* H;
uint32_t cb(uint32_t rva) { return 0xB2000000u | rva; }
uint32_t alloc(uint32_t pool, uint32_t size, uint32_t line, uint32_t zero) { return H->h(0xA2E6, 0, pool, size, 0, 0, 0xB20FBF2Cu, line, zero); }
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_687d0(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[1], (const uint32_t*)(uintptr_t)r[2]);
    H = &e;
    const D2Game687D0Env env = { cb, alloc };
    D2Game_687D0((uint8_t*)(uintptr_t)r[0], env);
    r[4] = 0;
}
