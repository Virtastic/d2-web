// 1.13c D2Game+0x84A90: AI think step with a charge counter (fastcall: ECX = game, EDX = unit, stack = AI tick record, ret 4).
// Record: +0 control (+0x14 counter, +0x18 phase, +0x1C search count), +8 target, +0x14 distance, +0x18 combat flag, +0x1C MonStats row.
// Row words by game+0x6D: +0x56 health threshold, +0x5C command roll, +0x62 attack roll, +0x68 hold roll, +0x6E counter limit,
// +0x74 sound variant, +0x7A mode split; byte +0x4F + difficulty, word +0x170 and byte +0x180 (the skill and its flag).
#include <cstdint>
#include <emscripten.h>
#include "d2game_aiext_env.h"
using namespace aiext;

// 1.13c D2Game+0x84A90
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_84a90_port(const D2Game77dEnv* e, const D2GameAiExt* x, uint32_t game, uint32_t unit, uint32_t task) {
    const uint32_t ctl = rd32(task, 0), target = rd32(task, 8), dist = rd32(task, 0x14), flag = rd32(task, 0x18), row = rd32(task, 0x1c);
    const uint32_t d = rd8(game, 0x6d);
    if (rd32(ctl, 0x14) == 0) {
        e->th2A3CA(unit, 0x5a, 0);
        e->th2A3CA(unit, 0x5b, 0);
        wr32(ctl, 0x18, 0);
    }
    const int32_t n = (int32_t)rd32(ctl, 0x14) + 1;
    wr32(ctl, 0x14, (uint32_t)n);
    const int32_t limit = rdS16(row, 0x6e + d * 2);
    if (n == limit) {
        x->th2A83E(unit, rdS16(row, 0x74 + d * 2) == 1 ? 0x96 : 0x2e, 0);
        e->subF22C0(game, (uint32_t)rd8(row, 0x4f + d) + 1u);
        return;
    }
    if (limit < n) {
        e->th2A3CA(unit, rdS16(row, 0x74 + d * 2) == 1 ? 0x5a : 0x5b, 1);
        wr32(ctl, 0x18, 1);
    }
    if ((int32_t)rd32(ctl, 0x1c) < 7 && (int32_t)e->th2A718(unit) < rdS16(row, 0x56 + d * 2)) {
        uint32_t buf[2] = { 0, 0x7fffffff };
        e->subB17B0m(1, (uint32_t)(uintptr_t)buf, game, unit, 0x7b9f0);
        if (buf[0] != 0) { e->subF2540(game, unit, buf[0], 2, 0, 0, 1, 0); return; }
        wr32(ctl, 0x1c, rd32(ctl, 0x1c) + 1);
    }
    if ((int32_t)dist > 4 && rd32(ctl, 0x18) == 0 && (int32_t)roll100(unit) < rdS16(row, 0x5c + rd8(game, 0x6d) * 2u)) {
        e->subF26B0(0, unit, game, target, 0);
        return;
    }
    if (flag == 0) {
        if (rd32(ctl, 0x18) != 0 || (int32_t)roll100(unit) < rdS16(row, 0x68 + rd8(game, 0x6d) * 2u)) {
            e->subF2540(game, unit, target, 2, 0, 0, 1, 0);
            return;
        }
    } else {
        if (rd32(ctl, 0x18) == 1 && rdS16(row, 0x170) >= 0) {
            e->subF2380(rd8(row, 0x180), unit, game, (uint32_t)rdS16(row, 0x170), target, 0, 0);
            wr32(ctl, 0x14, 0); wr32(ctl, 0x18, 0);
            return;
        }
        if ((int32_t)roll100(unit) < rdS16(row, 0x62 + rd8(game, 0x6d) * 2u)) {
            const int32_t r = (int32_t)roll(unit, 100);
            e->subF2220(r < rdS16(row, 0x7a + rd8(game, 0x6d) * 2u) ? 5 : 4, unit, game, target);
            return;
        }
    }
    const int32_t a = rdS16(row, 0x6e + rd8(game, 0x6d) * 2u);
    int32_t b = 0x18 - a;
    if (b < 6) b = 6;
    if (a + b < (int32_t)rd32(ctl, 0x14)) { wr32(ctl, 0x14, 0); wr32(ctl, 0x18, 0); }
    e->subF22C0(game, 0xf);
}
