// 1.13c D2Game+0x86900: AI think step of a skill-casting monster that retreats when hurt (fastcall: ECX = game, EDX = unit, stack =
// AI tick record, ret 4). Record +0 control (+0x14 phase 1/2, +0x18 longest distance seen under 30, +0x1C skill cooldown), +8 target,
// +0x14 distance, +0x18 combat flag, +0x1C MonStats row. Row word +0x6E (by game+0x6D) is a skill mask (bit 0 skills +0x170/+0x176,
// bit 1 skill +0x172, bit 2 skill +0x174; mode bytes +0x180..+0x183); words +0x56 attack, +0x5C act, +0x62 range, +0x68 cast rolls.
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

// 1.13c D2Game+0x86900
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_86900_port(const D2Game77dEnv* e, const D2GameAiExt* x, const D2GameO2Ext* o,
                                                       uint32_t game, uint32_t unit, uint32_t task) {
    const uint32_t ctl = rd32(task, 0), flag = rd32(task, 0x18), target = rd32(task, 8), row = rd32(task, 0x1c);
    const int32_t dist = (int32_t)rd32(task, 0x14);
    const uint32_t d = rd8(game, 0x6d) * 2u;
    auto w = [&](uint32_t off) { return rdS16(row, off + d); };
    const int32_t mask = w(0x6e);
    const bool b1 = mask & 1, b2 = mask & 2, b4 = mask & 4;
    if ((int32_t)rd32(ctl, 0x1c) > 0) wr32(ctl, 0x1c, rd32(ctl, 0x1c) - 1);
    uint32_t foeDist = 0;
    const uint32_t foe = x->subB1BC0(unit, game, 0, (uint32_t)(uintptr_t)&foeDist);
    // 0xF2380 with EAX = hi (what the site last loaded) under the mode byte
    auto cast = [&](uint32_t skOff, uint32_t modeOff, uint32_t hi, uint32_t who) {
        o->subF2380a((hi & 0xffffff00u) | rd8(row, modeOff), unit, game, (uint32_t)rdS16(row, skOff), who, 0, 0);
    };
    auto sk170 = [&](uint32_t hi, uint32_t who) { cast(0x170, 0x180, hi, who); };
    auto sk176 = [&](uint32_t who) { cast(0x176, 0x183, (uint32_t)rdS16(row, 0x176), who); };
    auto cooldown = [&](uint32_t skOff, uint32_t modeOff) {   // skills +0x172/+0x174 on the target, then 11 ticks of cooldown
        cast(skOff, modeOff, target, target);
        wr32(ctl, 0x1c, 0xb);
    };
    auto circle = [&] { e->subF26B0(0, unit, game, target, 4); };
    auto close = [&] { e->subF2540(game, unit, target, 2, 0, 0, 1, 7); };
    auto bit2 = [&] { return b2 && (int32_t)rd32(ctl, 0x1c) <= 0 && (int32_t)roll(unit, 100) < w(0x68); };
    auto bit4 = [&] { return b4 && (int32_t)rd32(ctl, 0x1c) <= 0 && (int32_t)roll(unit, 100) < w(0x68); };
    auto foeCast = [&](uint32_t hi170) {   // the foe in range: skill +0x170 or +0x176 at even odds
        if ((int32_t)roll(unit, 100) < 0x32) sk170(hi170, foe); else sk176(foe);
    };

    if (slowType(unit)) {
        if (rd32(ctl, 0x14) == 0) wr32(ctl, 0x14, 1);
        if (dist < 0x1e && dist > (int32_t)rd32(ctl, 0x18)) wr32(ctl, 0x18, (uint32_t)dist);
        if (flag != 0) {
            if (roll100(unit) <= 0x1e && b1) {
                if ((int32_t)roll(unit, 100) < 0x32) sk170(target, target); else sk176(target);
                return;
            }
            postCmd(e, game, unit, target, 4, 0x10);
            return;
        }
    }
    const int32_t hp = (int32_t)e->th2A718(unit);
    if (rd32(ctl, 0x14) == 2) {
        if (hp >= 0x4b) { wr32(ctl, 0x14, 1); close(); return; }
        if (dist < 0xe || dist <= (int32_t)rd32(ctl, 0x18)) {
            int32_t bonus = 0;
            const int32_t den = rdS16(row, 0x32);
            if (den > 0) {
                const int32_t q = rdS16(row, 0x34) * 100 / den - 100;
                bonus = q <= 0 ? 0 : (q >= 0x78 ? 0x78 : q);
            }
            if (bonus < -0x7e || bonus > 0x7e) { x->fatal(); __builtin_trap(); }
            if (bonus != 0) wr32(mdata(unit), 0x1c, (uint32_t)bonus);
            if (e->subF2D80(target, game, unit, 8, 1) != 0) return;
        }
        if (dist >= w(0x62) || (int32_t)roll(unit, 100) >= w(0x5c)) { e->subF22C0(game, 0xf); return; }
        if (bit2()) { cooldown(0x172, 0x181); return; }
        if (bit4()) { cooldown(0x174, 0x182); return; }
        if (b1 && foe != 0 && (int32_t)foeDist <= 0x14) { foeCast(foe); return; }
        circle();
        return;
    }
    if (hp < 0x21) {
        wr32(ctl, 0x14, 2);
        if (e->subF2D80(target, game, unit, 8, 0) != 0) return;
    }
    if (flag != 0) {
        wr32(ctl, 0x14, 1);
        if ((int32_t)roll100(unit) < w(0x56)) {
            if (b1 && foe != 0 && (int32_t)roll(unit, 100) <= 0x1e) {
                if ((int32_t)foeDist <= 0x14) {
                    const uint32_t r = roll(unit, 100);
                    if ((int32_t)r < 0x32) sk170((uint32_t)rdS16(row, 0x170), foe);
                    else cast(0x176, 0x183, r, foe);
                    return;
                }
            } else {
                e->subF2220(4, unit, game, target);
                return;
            }
        }
        if ((int32_t)roll(unit, 100) < 0x21) circle(); else e->subF22C0(game, 0xa);
        return;
    }
    if (dist >= w(0x62)) {
        if (rd32(ctl, 0x14) == 1) close(); else e->subF22C0(game, 0xf);
        return;
    }
    wr32(ctl, 0x14, 1);
    if ((int32_t)roll(unit, 100) < w(0x5c)) {
        if (bit2()) { cooldown(0x172, 0x181); return; }
        if (bit4()) { cooldown(0x174, 0x182); return; }
        if (b1 && foe != 0 && (int32_t)foeDist <= 0x14) {
            if ((int32_t)roll(unit, 100) >= 0x4b) circle(); else foeCast(foe);
            return;
        }
        close();
        return;
    }
    if (dist > 0x14) { close(); return; }
    if (dist < 9 && (int32_t)roll(unit, 100) < 0x32) { e->subF2D80(target, game, unit, 8, 0); return; }
    if ((int32_t)roll(unit, 100) < 0x32) circle(); else e->subF22C0(game, 0xa);
}
