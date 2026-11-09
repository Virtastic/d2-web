// 1.13c D2Game+0x85B60: AI think step of a boss caster, a five-state machine kept in the control block (fastcall: ECX = game, EDX = unit,
// stack = AI tick record, ret 4). Record +0 control (+0x14 burst counter, +0x18 retarget counter, +0x1C state), +8 target, +0x14
// distance, +0x18 combat flag, +0x1C MonStats row. The hp percent from 0xA718 gives k = max(0, (100 - hp) / 5), which biases the rolls.
// Row skill words +0x170..+0x17E (mode bytes +0x180..+0x185); row word +0x56 (by game+0x6D) is the flagged-attack roll.
#include <cstdint>
#include <emscripten.h>
#include "d2game_o2_env.h"
using namespace aiext;

// 1.13c D2Game+0x85B60
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_85b60_port(const D2Game77dEnv* e, const D2GameAiExt* x, const D2GameO2Ext* o,
                                                       uint32_t game, uint32_t unit, uint32_t task) {
    (void)x;
    const uint32_t row = rd32(task, 0x1c), ctl = rd32(task, 0);
    const int32_t dist = (int32_t)rd32(task, 0x14);
    const uint32_t flag = rd32(task, 0x18);
    uint32_t target = rd32(task, 8);
    const int32_t hp = (int32_t)e->th2A718(unit);
    int32_t k = (int32_t)(100u - (uint32_t)hp) / 5;
    if (k < 0) k = 0;
    uint32_t state = rd32(ctl, 0x1c);
    auto done = [&] { wr32(ctl, 0x1c, state); };
    // EAX into 0xF2380 is what the site last loaded with the mode byte in AL
    auto cast = [&](uint32_t skOff, uint32_t modeOff, uint32_t hi, uint32_t who) {
        o->subF2380a((hi & 0xffffff00u) | rd8(row, modeOff), unit, game, (uint32_t)rdS16(row, skOff), who, 0, 0);
    };
    auto sext = [&](uint32_t off) { return (uint32_t)rdS16(row, off); };
    auto burst = [&] {   // state 2 with a burst of 3..5 ticks
        state = 2;
        wr32(ctl, 0x14, roll(unit, 3) + 3);
    };

    bool pick = false;   // fall into the no-dispatch tail (0x85CAE)
    if (flag != 0) {
        if ((int32_t)roll100(unit) <= rdS16(row, 0x56 + rd8(game, 0x6d) * 2u)) burst(); else state = 3;
    } else if (dist > 0x14) {
        state = 4;
    } else if (state != 0) {
        if (state > 4) pick = true;
    } else if (hp <= 0x14 && dist < 5 && (int32_t)roll100(unit) < 0x28) {
        state = 1;
    } else if ((int32_t)roll100(unit) < k + 0x32) {
        burst();
    } else {
        pick = true;
    }

    if (pick) {
        const uint32_t r = roll100(unit);
        state = 0;
        if (r >= 0x41) { e->subF22C0(game, 0xa); done(); return; }
        if ((int32_t)roll100(unit) >= 0x41 && dist <= 5) {
            wr8(mdata(unit), 0x20, 4);
            e->subF2540(game, unit, target, 2, 0, 0, 1, 7);
        } else {
            wander(unit, 4);
            e->subF2540(game, unit, target, 2, 0, 0, 1, 0);
        }
        done();
        return;
    }

    switch (state) {
    case 0:
        state = 4;
        e->subF22C0(game, 5);
        done();
        return;
    case 3: {
        state = 0;
        if ((int32_t)roll(unit, 100) < k + 0x50) {
            if ((int32_t)roll(unit, 100) < 0x50 - k) e->subF2220(4, unit, game, target);
            else cast(0x174, 0x182, sext(0x174), target);
            done();
            return;
        }
        wr32(mdata(unit), 0x1c, 0x32);
        if (o->subF26B0r(1, unit, game, target, 3) == 0) e->subF1B10(0xc, game, unit);
        done();
        return;
    }
    case 4: {
        state = 0;
        wr32(mdata(unit), 0x1c, 0x32);
        if (o->subF2870r(target, game, unit) != 0) { done(); return; }
        if (unit != 0) {
            for (uint32_t n = rd32(unit, 0xdc); n != 0;) {
                const uint32_t nx = rd32(n, 0x24);
                if (rd8(n, 0) == 2) o->sub8E6F0(n);
                n = nx;
            }
        }
        if (o->subF1D90r(0xc, target, game, unit, 2, 6) == 0) e->subF1B10(0xc, game, unit);
        done();
        return;
    }
    case 1:
        state = 0;
        wr32(mdata(unit), 0x1c, 0x32);
        if (e->subF2D80(target, game, unit, 8, 1) != 0) { done(); return; }
        if (flag != 0) { cast(0x174, 0x182, flag, target); done(); return; }
        [[fallthrough]];
    case 2: {
        const uint32_t c = rd32(ctl, 0x14) - 1;
        wr32(ctl, 0x14, c);
        if (c == 0) state = 0;
        const uint32_t n18 = rd32(ctl, 0x18);
        if (n18 == 0) {
            wr32(ctl, 0x18, 2);
            if (o->subF26B0r(1, unit, game, target, 3) == 0) e->subF1B10(0xc, game, unit);
            done();
            return;
        }
        wr32(ctl, 0x18, n18 + 1);
        target = o->sub84720(game, unit, row, target);
        int32_t n = 0;
        while (n < 8 && rdS16(row, 0x170 + 2 * n) >= 0) ++n;
        if (n <= 0) {
            e->subF1B10(6, game, unit);
        } else {
            const int32_t q = 100 / n;
            const int32_t r = (int32_t)roll100(unit);
            const uint8_t diff = rd8(game, 0x6d);
            bool near = false;
            if (diff != 0 && target != 0 && o->thAD78(unit, target, 4) != 0) near = true;
            if (diff != 0 && (near || dist > 0x1e)) cast(0x17a, 0x185, 0, target);
            else if (dist < 0xf && r < q && diff != 0) cast(0x178, 0x184, (uint32_t)q, target);
            else if (r < 2 * q) cast(0x176, 0x183, sext(0x176), target);
            else if (r < 3 * q) cast(0x172, 0x181, sext(0x172), target);
            else cast(0x170, 0x180, (uint32_t)q, target);
        }
        if ((int32_t)roll100(unit) < 0x32 - k) wr32(ctl, 0x18, 0);
        done();
        return;
    }
    }
}
