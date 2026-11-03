// 1.13c D2Game+0x87010 (AITHINK_Fn015_SandMaggot): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_179a0.h). tp: +0 control, +8 target, +0x18 combat flag, +0x1C monstats row. Control: +0x14 mode, +0x18 cooldown
// frame, +0x1C counter. frame = game +0xA8. Row words indexed by game byte +0x6D: +0x56 / +0x5C / +0x62 / +0x68 / +0x6E; skills +0x170 / +0x172 / +0x174 (words), skill bytes +0x180 / +0x181 / +0x182.
// 0xB1BC0 (EAX = unit, ECX = game, stack = &dist) returns the nearest target (near) and writes its distance. Idle is 0xF2040 (EAX = game, EBX = unit, stack = frames). Casts are 0xF2380
// (ECX = unit, EBX = game, stack skill, a, b, c); EAX low byte = the skill byte over residue.
// Flow: mode < 3 and no target: (near == 0 or dist > 10) and frame > cool and skill +0x172 >= 0: cast, idle 30, cool = frame + w6E, mode = 3. Then near != 0 and dist < 16 goes to the
// mode-3 branch: frame > cool and skill +0x170 >= 0: cast, idle 25, cool = frame + w6E, mode = 1; else idle 20 for mode 3. Otherwise life% (0xA718) < 25, skill +0x172 >= 0, dist < 7,
// frame > cool, roll < 20: cast, idle 30, mode 3. Combat flag: roll < w68 runs 0xA496(unit, 0) and the command {4, unit, target, 0, 0, flag << 8, 0, 0} (0x75E10; flag 0x65 when 0x1320(class)
// byte +0x104 & ga[+0x10]). near != 0, dist < 15, roll < w5C: 0xF2220 (EAX = 5, stack game, near). Roll < 20: a byte roll >= 0x80 picks 6 else 5, stored at the unit's record +0x18
// (the byte +0x20 := 6), 0xF2540 (ECX = game, EDX = unit, stack target, 2, 0, 0, 1, 0). Else counter >= w62 or roll >= w56: idle 12. Else mode 2 with skill +0x174 >= 0: counter++, mode = 1,
// cast, idle 20; else 0xF26B0 (EAX = 0, ECX = unit, stack game, target, 6), mode = 2.
#pragma once
#include <cstdint>
#include "d2game_179a0.h"

namespace g87010 {
template <class E> void SandMaggot(E& e, uint32_t game, uint32_t unit, uint32_t tp, uint32_t ga)
{
    // 1.13c D2Game+0x87010
    const uint32_t ctl = e.rd32(tp), tgt = e.rd32(tp + 8), row = e.rd32(tp + 0x1C);
    const uint32_t flag = e.rd32(tp + 0x18);
    const uint32_t d = e.rd8(game + 0x6D) * 2;
    const uint32_t frame = e.rd32(game + 0xA8);
    uint32_t dd = 0;
    const uint32_t nr = e.h(0xB1BC0, unit, game, 0, 0, 0, e.stk(&dd));
    const int32_t dist = (int32_t)dd;
    const uint32_t mode = e.rd32(ctl + 0x14);
    auto idle = [&](uint32_t n) { e.h(0xF2040, game, 0, 0, unit, 0, n); };
    auto finish = [&](uint32_t m) { e.wr32(ctl + 0x18, (uint32_t)e.rd16s(row + 0x6E + d) + frame); e.wr32(ctl + 0x14, m); };
    bool toA = false, toB = false;
    if (mode < 3) {
        if (tgt != 0) toB = true;
        else {
            if ((nr == 0 || dist > 10) && (int32_t)frame > (int32_t)e.rd32(ctl + 0x18) && e.rd16s(row + 0x172) >= 0) {
                e.h(0xF2380, (nr & ~0xFFu) | e.rd8(row + 0x181), unit, 0, game, 0, (uint32_t)e.rd16s(row + 0x172), 0, 0, 0);
                idle(0x1E);
                finish(3);
                return;
            }
            toA = true;
        }
    } else { toA = tgt == 0; toB = tgt != 0; }
    if (toA && nr != 0 && dist < 0x10) toB = true;
    bool toC = toA && !toB;
    if (toB && mode == 3) {
        if ((int32_t)frame > (int32_t)e.rd32(ctl + 0x18) && e.rd16s(row + 0x170) >= 0) {
            const uint32_t w = (uint32_t)e.rd16s(row + 0x170);
            e.h(0xF2380, (frame & 0xFFFF0000u) | (w & 0xFF00u) | e.rd8(row + 0x180), unit, 0, game, 0, w, tgt, 0, 0);
            idle(0x19);
            e.wr32(ctl + 0x18, (uint32_t)e.rd16s(row + 0x6E + d) + frame); e.wr32(ctl + 0x14, 1);
            return;
        }
        toC = true;
    }
    if (toC && mode == 3) { idle(0x14); return; }
    // 1.13c D2Game+0x870EF
    const int32_t life = (int32_t)e.h(0xA718, 0, 0, 0, 0, 0, unit);
    if (life < 0x19 && e.rd16s(row + 0x172) >= 0 && dist < 7 && (int32_t)frame > (int32_t)e.rd32(ctl + 0x18) && g179a0::Roll(e, unit) < 0x14) {
        const uint32_t w = (uint32_t)e.rd16s(row + 0x172);
        e.h(0xF2380, (w & ~0xFFu) | e.rd8(row + 0x181), unit, 0, game, 0, w, tgt, 0, 0);
        idle(0x1E);
        finish(3);
        return;
    }
    if (flag != 0) {
        if ((int32_t)g179a0::Roll(e, unit) < e.rd16s(row + 0x68 + d)) {
            e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
            uint32_t f = 0x64;
            if (unit) {
                const uint32_t ms = e.h(0x1320, e.rd32(unit + 4));
                if (ms && (e.rd8(ms + 0x104) & e.rd32(ga + 0x10))) f = 0x65;
            }
            uint32_t blk[8] = { 4, unit, tgt, 0, 0, f << 8, 0, 0 };
            e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(blk), 1);
            e.blk(0x75E10, blk, 8);
            return;
        }
    }
    if (nr != 0 && dist < 0xF && (int32_t)g179a0::Roll(e, unit) < e.rd16s(row + 0x5C + d)) {
        e.h(0xF2220, 5, 0, 0, 0, 0, game, nr);
        return;
    }
    if (g179a0::Roll(e, unit) < 0x14) {
        const uint64_t n = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5u + e.rd32(unit + 0x24);
        e.wr32(unit + 0x20, (uint32_t)n); e.wr32(unit + 0x24, (uint32_t)(n >> 32));
        const uint32_t side = ((uint32_t)n & 0xFF) < 0x80 ? 5u : 6u;
        uint32_t obj = 0;
        if (unit && e.rd32(unit) == 1 && e.rd32(unit + 0x14)) obj = e.rd32(e.rd32(unit + 0x14) + 0x2C);
        e.wr32(obj + 0x18, side);
        e.wr8(obj + 0x20, 6);
        e.h(0xF2540, 0, game, unit, 0, 0, tgt, 2, 0, 0, 1, 0);
        return;
    }
    if ((int32_t)e.rd32(ctl + 0x1C) >= e.rd16s(row + 0x62 + d) || (int32_t)g179a0::Roll(e, unit) >= e.rd16s(row + 0x56 + d)) { idle(0xC); return; }
    if (mode == 2 && e.rd16s(row + 0x174) >= 0) {
        e.wr32(ctl + 0x1C, e.rd32(ctl + 0x1C) + 1); e.wr32(ctl + 0x14, 1);
        const uint32_t w = (uint32_t)e.rd16s(row + 0x174);
        e.h(0xF2380, (tgt & ~0xFFu) | e.rd8(row + 0x182), unit, 0, game, 0, w, tgt, 0, 0);
        idle(0x14);
        return;
    }
    e.h(0xF26B0, 0, unit, 0, 0, 0, game, tgt, 6);
    e.wr32(ctl + 0x14, 2);
}
}
