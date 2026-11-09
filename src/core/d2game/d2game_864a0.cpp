// 1.13c D2Game+0x864A0: AI think step of a monster that seeks the nearest foe (fastcall: ECX = game, EDX = unit, stack = AI tick
// record, ret 4). Record +8 target, +0x14 distance, +0x18 combat flag, +0x1C MonStats row; row words by game+0x6D: +0x56 leap roll,
// +0x5C attack roll, +0x62 idle ticks, +0x68 close roll, +0x6E melee range (its low byte the F2540 step), +0x74/+0x7A skill rolls,
// +0x80 leap range (its low byte the leap argument); row words +0x170/+0x172/+0x174 skills with mode bytes +0x180/+0x181/+0x182.
#include <cstdint>
#include <emscripten.h>
#include "d2game_o2_env.h"
using namespace aiext;

namespace {
bool slowType(uint32_t unit) {   // monster data +0x54 is 3 or 0x13
    if (unit == 0 || rd32(unit, 0) != 1 || rd32(unit, 0x14) == 0) return false;
    const uint32_t t = rd32(rd32(unit, 0x14), 0x54);
    return t == 3 || t == 0x13;
}
}

// 1.13c D2Game+0x864A0
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_864a0_port(const D2Game77dEnv* e, const D2GameAiExt* x, const D2GameO2Ext* o,
                                                       uint32_t game, uint32_t unit, uint32_t task) {
    uint32_t dist = rd32(task, 0x14);
    const uint32_t flag = rd32(task, 0x18), target = rd32(task, 8), row = rd32(task, 0x1c);
    const uint32_t d = rd8(game, 0x6d) * 2u;
    auto w = [&](uint32_t off) { return rdS16(row, off + d); };
    const uint32_t foe = x->subB1BC0(unit, game, 0, (uint32_t)(uintptr_t)&dist);
    if (foe == 0) {
        if (roll100(unit) < 0x32) {
            wander(unit, 3);
            e->subF2540(game, unit, target, 2, 0, 0, 1, 0);
        } else {
            e->subF22C0(game, (uint32_t)w(0x62));
        }
        return;
    }
    if (flag == 0 && slowType(unit)) { postCmd(e, game, unit, foe, 4, 0x10); return; }
    if ((int32_t)dist < 6 && (int32_t)roll100(unit) < w(0x68)) {
        wr32(mdata(unit), 0x1c, 100);
        if (e->subF2C10(foe, unit, game, 0xc, 1) != 0) return;
    }
    const int32_t leap = w(0x80);
    if (leap > 0 && (int32_t)dist > leap && (int32_t)roll(unit, 100) < w(0x56)) {
        wr32(mdata(unit), 0x1c, 10);
        o->subF2F30(foe, unit, game, rd8(row, 0x80 + d));
        return;
    }
    if ((int32_t)dist > w(0x6e)) {
        wr32(mdata(unit), 0x1c, 100);
        uint8_t b = rd8(row, 0x6e + d);
        if (b == 0) b = 1;
        e->subF2540(game, unit, foe, 0xf, 0, 0, (dist & 0xffffff00u) | b, 0);
        return;
    }
    if ((int32_t)roll100(unit) >= w(0x5c)) { e->subF22C0(game, (uint32_t)w(0x62)); return; }
    // EAX keeps what the call site last loaded (the skill, or the foe for +0x174) under the mode byte
    auto cast = [&](uint32_t skOff, uint32_t modeOff, uint32_t hi) {
        o->subF2380a((hi & 0xffffff00u) | rd8(row, modeOff), unit, game, (uint32_t)rdS16(row, skOff), foe, 0, 0);
    };
    if (rdS16(row, 0x172) >= 0 && (int32_t)roll(unit, 100) < w(0x74)) { cast(0x172, 0x181, (uint32_t)rdS16(row, 0x172)); return; }
    if (rdS16(row, 0x174) >= 0 && (int32_t)roll(unit, 100) < w(0x7a)) { cast(0x174, 0x182, foe); return; }
    if (rdS16(row, 0x170) >= 0) { cast(0x170, 0x180, (uint32_t)rdS16(row, 0x170)); return; }
    e->subF2220(4, unit, game, foe);
}
