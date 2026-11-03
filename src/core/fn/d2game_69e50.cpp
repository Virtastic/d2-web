// 1.13c D2Game+0x69E50 probe: the port over the HookEnv recorder.
// Descriptor `regs ecx edi`, args 7: r[0] index, r[1] monster, r[2] handler ECX value, r[3] hook log, r[5] hook values, r[6] table (pinned 0x107D50), r[7] count (pinned 0x107E4C), r[4] result.
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"

struct D2Game69E50Env {
    void     (*fatal)(uint32_t line);
    uint32_t (*isBadCodePtr)(uint32_t fn);
    uint32_t (*call)(uint32_t fn, uint32_t ecx, uint32_t edx, uint32_t arg);
};
uint32_t D2Game_69E50(uint32_t ecx, uint32_t mon, uint32_t stackVal, uint32_t tbl, int32_t count, const D2Game69E50Env& e);

namespace {
HookEnv* H;
void fatal(uint32_t) { abort(); }
uint32_t bad(uint32_t) { return 0; }
uint32_t call(uint32_t fn, uint32_t c, uint32_t d, uint32_t a) { return H->h(HookEnv::handlerRva(fn), 0, c, d, 0, 0, a); }
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_69e50(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[3], (const uint32_t*)(uintptr_t)r[5]);
    H = &e;
    const D2Game69E50Env env = { fatal, bad, call };
    r[4] = D2Game_69E50(r[0], r[1], r[2], r[6], (int32_t)HookEnv::rd32(r[7]), env);
}
