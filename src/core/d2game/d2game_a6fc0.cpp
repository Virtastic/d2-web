// 1.13c D2Game+0xA6FC0: AI step, apply state 0x38 and dwell near the target. Fastcall ECX = game, EDX = unit, stack = AI task record (ret 4).
// Record: +0 control, +8 target, +0x14 range, +0x18 flag, +0x1C MonStats row; control +0x14 = wait limit (0 = 30), +0x1C = entered. Without unit
// state 0x38 the unit is reset (0x9D230), its type-2 events are cleared and event 2 is queued for the next tick. With it: a range above the wait
// limit idles 10 frames; a percent from row words +0x32/+0x34 (capped 0x78) goes into the AI data (+0x1C, mode +0x18 = 2); the swing starts
// through 0xF2C10 or 0xF2D80 by the 0x1560(class, 0xF) test; the entered state may start 0xF2220 instead, and a failed swing walks (0xF1B10, 6).
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <emscripten.h>
#include "d2game_a6fc0.h"

namespace {
inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 4); return v; }
inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)(a + o), &v, 4); }
inline int32_t rdS16(uint32_t a, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 2); return v; }
}

// 1.13c D2Game+0xA6FC0
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_a6fc0_port(const D2Game77dEnv* e, const GA6fc0Extra* x, uint32_t game, uint32_t unit, uint32_t task) {
    const uint32_t ctl = rd32(task, 0), target = rd32(task, 8), row = rd32(task, 0x1c), flag = rd32(task, 0x18);
    const int32_t range = (int32_t)rd32(task, 0x14);
    auto walk = [&] {
        if (unit == 0) return;
        for (uint32_t n = rd32(unit, 0xdc); n != 0;) {
            const uint32_t next = rd32(n, 0x24);
            if (*(const uint8_t*)(uintptr_t)n == 2) x->f8E6F0(n, game);
            n = next;
        }
    };
    const uint32_t data = (unit != 0 && rd32(unit, 0) == 1) ? rd32(unit, 0x14) : 0;
    const uint32_t cls = unit != 0 ? rd32(unit, 4) : 0xffffffffu;
    if (e->th2A68E(unit, 0x38) == 0) {
        x->fD230(game, unit, ctl, 0);
        walk();
        x->f8EA80(game, unit, 2, rd32(game, 0xa8) + 1, 0, 0, 0);
        return;
    }
    int32_t wait = (int32_t)rd32(ctl, 0x14);
    if (wait == 0) wait = 0x1e;
    if (range > wait) { e->subF22C0(game, 10); return; }
    int32_t pct = 0;
    const int32_t den = rdS16(row, 0x32);
    if (den > 0) {
        pct = rdS16(row, 0x34) * 100 / den;
        if (pct >= 100) { pct -= 100; if (pct > 0x78) pct = 0x78; } else pct = 0;
    }
    const bool entered = rd32(ctl, 0x1c) != 0;
    if (!entered) {
        wr32(ctl, 0x1c, 1);
        if (data != 0) { wr32(data, 0x38, 0); wr32(data, 0x34, 0); }
        walk();
    } else if (flag != 0 && x->f15D0(cls, 9) == 0 && x->f1560(cls, 4) != 0) {
        e->subF2220(4, unit, game, target);
        return;
    }
    if (pct < -0x7e || pct > 0x7e) std::abort();   // retail logs assert line 0x110 and exits
    const uint32_t ai = data != 0 ? rd32(data, 0x2c) : 0;
    if (ai != 0) { wr32(ai, 0x18, 2); if (pct != 0) wr32(ai, 0x1c, (uint32_t)pct); }
    const uint32_t one = entered ? 1 : 0;
    const uint32_t r = x->f1560(cls, 0xf) != 0 ? e->subF2C10(target, unit, game, 0x1e, one) : e->subF2D80(target, game, unit, 0x1e, one);
    if (entered && r == 0) e->subF1B10(6, game, unit);
}
