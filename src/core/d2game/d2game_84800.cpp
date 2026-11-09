// 1.13c D2Game+0x84800: AI think step, a charge-or-wander melee (fastcall: ECX = game, EDX = unit, stack = AI tick record, ret 4).
// Record +8 target, +0x18 combat flag, +0x1C MonStats row; row words by game+0x6D: +0x56 charge roll, +0x5C command roll,
// +0x62 search radius, +0x68 idle frames.
#include <cstdint>
#include <emscripten.h>
#include "d2game_aiext_env.h"
using namespace aiext;

// 1.13c D2Game+0x84800
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_84800_port(const D2Game77dEnv* e, const D2GameAiExt* x, uint32_t game, uint32_t unit, uint32_t task) {
    const uint32_t target = rd32(task, 8), row = rd32(task, 0x1c);
    const uint32_t d = rd8(game, 0x6d) * 2u;
    if (rd32(task, 0x18) != 0) {
        if ((int32_t)roll100(unit) < rdS16(row, 0x5c + d)) { postCmd(e, game, unit, target, 4, 0x10); return; }
        e->subF22C0(game, (uint32_t)rdS16(row, 0x68 + d));
        return;
    }
    if ((int32_t)roll100(unit) < rdS16(row, 0x56 + d)) {
        wr32(mdata(unit), 0x1c, 0x4b);
        x->subF3000(target, unit, game, 7);
        return;
    }
    uint32_t buf[2] = { 0, 0x7fffffff };
    e->subB17B0m(1, (uint32_t)(uintptr_t)buf, game, unit, 0x9ca00);
    const int32_t r = rdS16(row, 0x62 + d);
    if (buf[0] != 0 && r * r < (int32_t)buf[1]) {
        wr32(mdata(unit), 0x1c, 0x4b);
        e->subF2540(game, unit, buf[0], 2, 0, 0, 1, 7);
        return;
    }
    if (roll100(unit) < 0x19) {
        wander(unit, 3);
        e->subF2540(game, unit, target, 2, 0, 0, 1, 0);
        return;
    }
    e->subF22C0(game, (uint32_t)rdS16(row, 0x68 + d));
}
