// 1.13c D2Game+0x83550: AI think step with a two-phase special move (fastcall: ECX = game, EDX = unit, stack = AI tick record, ret 4).
// Record +0 control (+0x14 = phase latch), +8 target, +0x14 move count, +0x18 combat flag, +0x1C MonStats row; row words by
// game+0x6D: +0x56 special roll, +0x5C retreat roll, +0x62 command roll, +0x68 monster-data speed (-126..126, else fatal), +0x6E the
// 0xF1A20 speed. 0xF1A20 and 0x1180 run inline (pure on the unit).
#include <cstdint>
#include <emscripten.h>
#include "d2game_aiext_env.h"
using namespace aiext;

namespace {
// 0xF1A20(eax = unit, edx = mode, cl = speed byte, esi = value): mode 1 -> 7; monster data +0x18 = mode, +0x1C = value, +0x20 = min(cl, 0x4D)
// (each only when non-zero); a value outside -126..126 is the fatal exit.
void f1a20(const D2GameAiExt* x, uint32_t unit, uint32_t mode, uint8_t cl, int32_t value) {
    if (value <= -0x7f || value >= 0x7f) { x->fatal(); return; }
    if (mode == 1) mode = 7;
    const uint32_t m = mdata(unit);
    if (mode != 0) wr32(m, 0x18, mode);
    if (value != 0) wr32(m, 0x1c, (uint32_t)value);
    if (cl != 0) wr8(m, 0x20, cl > 0x4d ? 0x4d : cl);
}
}

// 1.13c D2Game+0x83550
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_83550_port(const D2Game77dEnv* e, const D2GameAiExt* x, uint32_t game, uint32_t unit, uint32_t task) {
    const uint32_t ctl = rd32(task, 0), target = rd32(task, 8), row = rd32(task, 0x1c), flag = rd32(task, 0x18);
    const int32_t count = (int32_t)rd32(task, 0x14);
    const uint32_t d = rd8(game, 0x6d) * 2u;
    if (rd32(ctl, 0x14) == 1 && flag != 0) {
        wr32(ctl, 0x14, 0);
        postCmd(e, game, unit, target, 4, 0x10);
        return;
    }
    wr32(ctl, 0x14, 0);
    auto approach = [&] {
        const int32_t v = rdS16(row, 0x68 + d);
        if (v < -0x7e || v > 0x7e) { x->fatal(); return; }
        if (v != 0) wr32(mdata(unit), 0x1c, (uint32_t)v);
        if (e->subF2D80(target, game, unit, 4, 1) == 0) e->subF2220(4, unit, game, target);
    };
    if (flag != 0) {
        if ((int32_t)roll100(unit) < rdS16(row, 0x62 + d)) { postCmd(e, game, unit, target, 4, 0x10); return; }
        approach();
        return;
    }
    if ((int32_t)roll100(unit) < rdS16(row, 0x56 + d)) {
        f1a20(x, unit, 0, (uint8_t)count, rdS16(row, 0x6e + d));
        wr32(ctl, 0x14, 1);
        e->subF2540(game, unit, target, 2, 0, 0, 1, 0);
        return;
    }
    if (count > 3) {
        if ((int32_t)roll(unit, 100) < rdS16(row, 0x5c + d)) {
            wr32(mdata(unit), 0x1c, 0xffffffce);
            e->subF1B10(4, game, unit);
            return;
        }
        e->subF1B10(3, game, unit);
        return;
    }
    approach();
}
