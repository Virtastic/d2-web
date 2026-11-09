// 1.13c D2Game+0x82990: AI think step of a monster with a timed velocity boost (fastcall: ECX = game, EDX = unit, stack = AI tick
// record, ret 4). Record +0 control (+0x14 boost ticks left, +0x18 walk latch, +0x1C the velocity delta added), +8 target,
// +0x14 distance, +0x18 combat flag, +0x1C MonStats row; row words by game+0x6D: +0x56 health gate, +0x5C/+0x62 retreat rolls,
// +0x68 approach roll, +0x6E velocity percent (eighths). Row words +0x32/+0x34 give the speed bonus (percent over 100, at most 120).
// 0x1180 runs inline (pure on the unit seed).
#include <cstdint>
#include <emscripten.h>
#include "d2game_o2_env.h"
using namespace aiext;

namespace {
[[noreturn]] void die(const D2GameAiExt* x) { x->fatal(); __builtin_trap(); }
// mdata+0x18 = mode, +0x1C = bonus when non-zero (a bonus outside -126..126 is the fatal exit)
void setMode(const D2GameAiExt* x, uint32_t unit, uint32_t mode, int32_t bonus) {
    if (bonus < -0x7e || bonus > 0x7e) die(x);
    const uint32_t m = mdata(unit);
    wr32(m, 0x18, mode);
    if (bonus != 0) wr32(m, 0x1c, (uint32_t)bonus);
}
bool slowType(uint32_t unit) {   // monster data +0x54 is 3 or 0x13
    if (unit == 0 || rd32(unit, 0) != 1 || rd32(unit, 0x14) == 0) return false;
    const uint32_t t = rd32(rd32(unit, 0x14), 0x54);
    return t == 3 || t == 0x13;
}
}

// 1.13c D2Game+0x82990
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_82990_port(const D2Game77dEnv* e, const D2GameAiExt* x, const D2GameO2Ext* o,
                                                       uint32_t game, uint32_t unit, uint32_t task) {
    const uint32_t ctl = rd32(task, 0), flag = rd32(task, 0x18), target = rd32(task, 8), row = rd32(task, 0x1c);
    const int32_t count = (int32_t)rd32(task, 0x14);
    const int32_t hp = (int32_t)e->th2A718(unit);
    const uint32_t d = rd8(game, 0x6d) * 2u;
    int32_t bonus = 0;
    const int32_t den = rdS16(row, 0x32);
    if (den > 0) {
        const int32_t q = rdS16(row, 0x34) * 100 / den;
        if (q >= 100) bonus = q - 100 > 0x78 ? 0x78 : q - 100;
    }
    auto approach = [&](bool hit) { e->subF2220(hit ? 4 : 5, unit, game, target); };
    if (rd32(ctl, 0x14) != 0) {
        const uint32_t left = rd32(ctl, 0x14) - 1;
        wr32(ctl, 0x18, 0);
        wr32(ctl, 0x14, left);
        if (left == 0 || hp > 0x4b) {
            const uint32_t v = o->thA42A(unit, 0x4a, 0);
            o->thA472(unit, 0x4a, v - rd32(ctl, 0x1c), 0);
        }
        if (flag == 0) {
            if (hp > 0x4b) {
                wr32(ctl, 0x14, 0);
                setMode(x, unit, 0xd, bonus);
                x->subF3000(target, unit, game, 7);
                return;
            }
        } else if (roll100(unit) < 0x21) {
            approach((int32_t)roll(unit, 100) < rdS16(row, 0x68 + d));
            return;
        }
        if (count >= 0x18 && !slowType(unit)) {
            if ((int32_t)roll(unit, 100) < 0x21) e->subF26B0(0, unit, game, target, 4);
            o->subF22C0s(game, unit, 0x14);
            return;
        }
        setMode(x, unit, 2, bonus);
        if (e->subF2D80(target, game, unit, 0xf, 1) != 0) return;
        if (flag != 0) { approach((int32_t)roll(unit, 100) < rdS16(row, 0x68 + d)); return; }
        e->subF1B10(5, game, unit);
        return;
    }
    if (flag == 0) {
        wr32(mdata(unit), 0x18, 0xd);
        e->subF2540(game, unit, target, 2, 0, 0, 1, 7);
        return;
    }
    bool walk = false;
    if (slowType(unit)) {
        if (hp < rdS16(row, 0x56 + d) && (int32_t)roll(unit, 100) < 0x32) {
            wr32(ctl, 0x14, roll(unit, 5) + 2);
            const uint32_t v = o->thA42A(unit, 0x4a, 0);
            if (v != 0) {
                const int32_t t = (int32_t)((uint32_t)rdS16(row, 0x6e + d) * v) / 8;
                wr32(ctl, 0x1c, (uint32_t)t);
                o->thA472(unit, 0x4a, (uint32_t)t + v, 0);
            } else {
                wr32(ctl, 0x1c, 0);
            }
            setMode(x, unit, 2, bonus);
            e->subF2D80(target, game, unit, 0xf, 0);
            return;
        }
        if (rd32(ctl, 0x18) == 0) walk = true;
        else if ((int32_t)roll(unit, 100) < 0x14) {
            e->subF26B0(0, unit, game, target, 3);
            wr32(ctl, 0x18, 0);
            return;
        }
    }
    if (!walk && rd32(ctl, 0x18) != 0 && (int32_t)roll100(unit) >= rdS16(row, 0x62 + d)) {
        if ((int32_t)roll(unit, 100) < rdS16(row, 0x5c + d)) {
            e->subF26B0(0, unit, game, target, 3);
            wr32(ctl, 0x18, 0);
        }
        o->subF22C0s(game, unit, 0xf);
        return;
    }
    wr32(ctl, 0x18, 1);
    approach((int32_t)roll100(unit) < rdS16(row, 0x68 + d));
}
