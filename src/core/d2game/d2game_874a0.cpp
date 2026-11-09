// 1.13c D2Game+0x874A0: AI think step for a ranged caster that keeps its distance (fastcall: ECX = game, EDX = unit, stack = AI tick
// record, ret 4). Record +8 target, +0x14 distance, +0x18 combat flag, +0x1C MonStats row; row words by game+0x6D: +0x56 health
// threshold (0x2A718 = Ordinal_11027), +0x5C step-back roll, +0x62 near attack roll, +0x68 mid attack roll. Monster-data +0x54 mode 3 or
// 0x13 posts command 5.
#include <cstdint>
#include <emscripten.h>
#include "d2game_aiext_env.h"
using namespace aiext;

// 1.13c D2Game+0x874A0
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_874a0_port(const D2Game77dEnv* e, const D2GameAiExt* x, uint32_t game, uint32_t unit, uint32_t task) {
    const uint32_t target = rd32(task, 8), row = rd32(task, 0x1c), flag = rd32(task, 0x18);
    uint32_t dist = rd32(task, 0x14);
    if (flag == 0 && unit != 0 && rd32(unit, 0) == 1 && rd32(unit, 0x14) != 0) {
        const uint32_t mode = rd32(rd32(unit, 0x14), 0x54);
        if (mode == 3 || mode == 0x13) { postCmd(e, game, unit, target, 5, 0x14); return; }
    }
    uint32_t out[1] = { 0 };
    const int32_t hp = (int32_t)e->th2A718(unit);
    if (hp >= rdS16(row, 0x56 + rd8(game, 0x6d) * 2u)) {
        if (flag != 0) { postCmd(e, game, unit, target, 4, 0x10); return; }
        if ((int32_t)dist < 0xf && x->subB1BC0(unit, game, (uint32_t)(uintptr_t)out, (uint32_t)(uintptr_t)&dist) != 0 &&
            (int32_t)roll(unit, 100) < rdS16(row, 0x62 + rd8(game, 0x6d) * 2u)) {
            e->subF2220(5, unit, game, target);
            return;
        }
        e->subF2540(game, unit, target, 2, 0, 0, 1, 7);
        return;
    }
    if ((int32_t)dist < 3) {
        wr32(mdata(unit), 0x1c, 0x32);
        if (e->subF2D80(target, game, unit, 5, 1) == 0) e->subF2220(5, unit, game, target);
        return;
    }
    if ((int32_t)dist > 0xf) { e->subF2540(game, unit, target, 2, 0, 0, 6, 0); return; }
    if (x->subB1BC0(unit, game, (uint32_t)(uintptr_t)out, (uint32_t)(uintptr_t)&dist) != 0 &&
        (int32_t)roll(unit, 100) < rdS16(row, 0x68 + rd8(game, 0x6d) * 2u)) {
        e->subF2220(5, unit, game, target);
        return;
    }
    if ((int32_t)roll(unit, 100) >= rdS16(row, 0x5c + rd8(game, 0x6d) * 2u)) { e->subF22C0(game, 10); return; }
    e->subF26B0(0, unit, game, target, 3);
}
