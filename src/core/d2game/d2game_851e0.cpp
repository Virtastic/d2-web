// 1.13c D2Game+0x851E0: AI think step that picks the nearest foe and charges, strikes or closes in (fastcall: ECX = game, EDX = unit,
// stack = AI tick record, ret 4). Record +8 target, +0x14 distance, +0x1C MonStats row; row words by game+0x6D: +0x56 close roll,
// +0x5C charge distance, +0x62 charge roll, +0x68 strike distance, +0x6E strike roll, +0x74 close distance, +0x7A wander roll,
// +0x80 idle frames. 0xB1BC0 picks the foe and writes the distance; the strike tries 0xF2D80 on the foe, then walks to the target.
#include <cstdint>
#include <emscripten.h>
#include "d2game_aiext_env.h"
using namespace aiext;

// 1.13c D2Game+0x851E0
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_851e0_port(const D2Game77dEnv* e, const D2GameAiExt* x, uint32_t game, uint32_t unit, uint32_t task) {
    const uint32_t row = rd32(task, 0x1c), target = rd32(task, 8);
    uint32_t dist = rd32(task, 0x14);
    auto w = [&](uint32_t base) { return rdS16(row, base + rd8(game, 0x6d) * 2u); };
    // the charge: monster data +0x1C = 10, then 0xF2540 with stack word 4 = the distance dword with its low byte replaced by the
    // charge-distance word's low byte (1 when that byte is 0)
    auto charge = [&](uint32_t who) {
        wr32(mdata(unit), 0x1c, 10);
        uint8_t b = (uint8_t)w(0x5c);
        if (b == 0) b = 1;
        dist = (dist & 0xffffff00u) | b;
        e->subF2540(game, unit, who, 2, 0, 0, dist, 0);
    };
    const uint32_t foe = x->subB1BC0(unit, game, 0, (uint32_t)(uintptr_t)&dist);
    if (foe != 0) {
        if ((int32_t)dist > w(0x5c) && (int32_t)roll100(unit) < w(0x62)) { charge(foe); return; }
        if ((int32_t)dist <= w(0x68) && (int32_t)roll100(unit) < w(0x6e)) {
            wr32(mdata(unit), 0x1c, 0x19);
            if (e->subF2D80(foe, game, unit, 5, 1) != 0) return;
            e->subF2220(4, unit, game, target);
            return;
        }
        if ((int32_t)dist < w(0x74) && (int32_t)roll100(unit) < w(0x56)) { e->subF2220(4, unit, game, foe); return; }
    }
    if ((int32_t)dist > w(0x5c) && (int32_t)roll100(unit) < w(0x62)) { charge(target); return; }
    if ((int32_t)roll100(unit) < w(0x7a)) {
        wander(unit, 4);
        e->subF2540(game, unit, target, 2, 0, 0, 1, 0);
        return;
    }
    e->subF22C0(game, (uint32_t)w(0x80));
}
