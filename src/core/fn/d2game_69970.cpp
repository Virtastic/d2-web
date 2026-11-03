// 1.13c D2Game+0x69970 probe: the port over the HookEnv recorder.
// Descriptor `regs ecx edx`, args 6: r[0] game, r[1] unit, r[2] hook log, r[3] hook values, r[5] table (pinned 0x107B8C), r[6] count (pinned 0x107BB4).
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"

struct D2Game69970Env {
    void     (*fatal)(uint32_t line);
    uint32_t (*unitRoom)(uint32_t unit);
    uint32_t (*roomLevel)(uint32_t room);
    uint32_t (*isBadCodePtr)(uint32_t fn);
    void     (*call)(uint32_t fn, uint32_t ecx, uint32_t edx);
};
void D2Game_69970(uint32_t game, uint32_t unit, uint32_t tbl, int32_t count, const D2Game69970Env& e);

namespace {
HookEnv* H;
void fatal(uint32_t) { abort(); }
uint32_t room(uint32_t u) { return H->h(0xA478, 0, 0, 0, 0, 0, u); }
uint32_t level(uint32_t rm) { return H->h(0xA556, 0, 0, 0, 0, 0, rm); }
uint32_t bad(uint32_t) { return 0; }
void call(uint32_t fn, uint32_t c, uint32_t d) { H->h(HookEnv::handlerRva(fn), 0, c, d); }
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_69970(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[2], (const uint32_t*)(uintptr_t)r[3]);
    H = &e;
    const D2Game69970Env env = { fatal, room, level, bad, call };
    D2Game_69970(r[0], r[1], r[5], (int32_t)HookEnv::rd32(r[6]), env);
    r[4] = 0;
}
