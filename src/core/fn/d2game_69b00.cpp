// 1.13c D2Game+0x69B00 probe: the port over the HookEnv recorder.
// Descriptor `regs eax ecx edx`, args 9: r[0] item, r[1] game, r[2] unit, r[3] out, r[5] multiplier, r[6] hook log, r[7] hook values,
// r[8] data tables (what the pinned slot 0xF829C reaches), r[9] class record (what #A928 returns), r[4] result.
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"

struct D2Game69B00Env {
    void     (*fatal)(uint32_t line);
    uint32_t (*unitValue)(uint32_t unit);
    uint32_t (*invItem)(uint32_t inv);
    uint32_t (*itemKind)(uint32_t item);
    uint32_t (*calcA)(uint32_t unit, uint32_t item, uint32_t game, uint32_t z);
    uint32_t (*calcB)(uint32_t unit, uint32_t stat, uint32_t z);
    uint32_t (*record)(uint32_t game);
    uint32_t (*dataTables)();
};
void D2Game_69B00(uint32_t item, uint32_t game, uint32_t unit, uint32_t out, uint32_t mul, const D2Game69B00Env& e);

namespace {
HookEnv* H;
uint32_t DT, REC;
void fatal(uint32_t) { abort(); }
uint32_t unitValue(uint32_t u) { return H->h(0xA7F0, 0, 0, 0, 0, 0, u); }
uint32_t invItem(uint32_t i) { return H->h(0xA54A, 0, 0, 0, 0, 0, i); }
uint32_t itemKind(uint32_t i) { return H->h(0xAA66, 0, 0, 0, 0, 0, i); }
uint32_t calcA(uint32_t u, uint32_t i, uint32_t g, uint32_t z) { return H->h(0xA80E, 0, 0, 0, 0, 0, u, i, g, z); }
uint32_t calcB(uint32_t u, uint32_t s, uint32_t z) { return H->h(0xA802, 0, 0, 0, 0, 0, u, s, z); }
uint32_t record(uint32_t g) { H->h(0xA928, 0, g); return REC; }
uint32_t dataTables() { return DT; }
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_69b00(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[6], (const uint32_t*)(uintptr_t)r[7]);
    H = &e; DT = r[8]; REC = r[9];
    const D2Game69B00Env env = { fatal, unitValue, invItem, itemKind, calcA, calcB, record, dataTables };
    D2Game_69B00(r[0], r[1], r[2], r[3], r[5], env);
    r[4] = 0;
}
