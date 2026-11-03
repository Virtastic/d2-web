// 1.13c D2Game+0x7EA10 (AITHINK_Fn055_Izual): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_179a0.h). tp: +0 control, +8 target, +0x14 target distance, +0x18 combat flag, +0x1C monstats row.
// Row words indexed by game byte +0x6D: +0x56 / +0x5C / +0x62 / +0x68 / +0x6E / +0x74, byte +0x4F + diff, skill word +0x170, skill byte +0x180. Control: +0x14 first-run flag,
// +0x18 pending idle, +0x1C cooldown. The target's X / Y come from PosWord (types 2, 4, 5 read the record at +0x2C directly, others ask 0xA454 / 0xA3B8). First run: ctl+0x14 := 1,
// 0xBA080 (EAX = game). Pending idle: 0xF22C0 (EAX = game, ESI = unit, stack = value), cleared, done. Casts are 0xF2380 (ECX = unit, EBX = game, stack skill, target, X, Y); EAX low
// byte = row +0x180, high bytes = X in the combat branch and the sign-extended skill in the out-of-combat branch.
// Combat: cooldown <= 0: roll >= w56: cooldown := 0; skill >= 0 and a second roll < w68: cast, idle := w6E, cooldown := w74, done; else idle (byte row+0x4F+diff). Otherwise
// (also cooldown > 0): cooldown--, 0xA496(unit, 0) and the command {4, unit, target, 0, 0, flag << 8, 0, 0} (flag 0x65 when 0x1320(unit class) byte +0x104 & ga[+0x10], else 0x64) via 0x75E10.
// Out of combat: skill >= 0 and distance < 10: cooldown <= 0 and a roll < w62: cast (as above), idle w6E, cooldown w74. Then cooldown <= 0 and a roll >= w5C: distance <= 10 idles
// (byte), else 0xF1D90 (EAX = 6, ECX = target, stack game, unit, 2, 9). Else 0xF2540 (ECX = game, EDX = unit, stack target, 2, 0, 0, 1, 7).
#pragma once
#include <cstdint>
#include "d2game_179a0.h"
#include "d2game_5d1d0.h"

namespace g7ea10 {
template <class E> void Izual(E& e, uint32_t game, uint32_t unit, uint32_t tp, uint32_t ga)
{
    // 1.13c D2Game+0x7EA10
    const uint32_t ctl = e.rd32(tp), tgt = e.rd32(tp + 8), row = e.rd32(tp + 0x1C);
    const int32_t dist = (int32_t)e.rd32(tp + 0x14), combat = (int32_t)e.rd32(tp + 0x18);
    const uint32_t d = e.rd8(game + 0x6D) * 2;
    const uint32_t X = d2g5d1d0::PosWord(e, tgt, 0xC, 0xA454);
    const uint32_t Y = d2g5d1d0::PosWord(e, tgt, 0x10, 0xA3B8);
    if (e.rd32(ctl + 0x14) == 0) { e.wr32(ctl + 0x14, 1); e.h(0xBA080, game); }
    if (e.rd32(ctl + 0x18) != 0) {
        e.h(0xF22C0, game, 0, 0, 0, unit, e.rd32(ctl + 0x18));
        e.wr32(ctl + 0x18, 0);
        return;
    }
    const int32_t sk = e.rd16s(row + 0x170);
    auto idleB = [&]() { e.h(0xF22C0, game, 0, 0, 0, unit, e.rd8(row + 0x4F + d / 2)); };
    auto setCool = [&]() { e.wr32(ctl + 0x18, (uint32_t)e.rd16s(row + 0x6E + d)); e.wr32(ctl + 0x1C, (uint32_t)e.rd16s(row + 0x74 + d)); };
    if (combat) {
        if ((int32_t)e.rd32(ctl + 0x1C) < 1) {
            const uint32_t r = g179a0::Roll(e, unit);
            if ((int32_t)r >= e.rd16s(row + 0x56 + d)) {
                e.wr32(ctl + 0x1C, 0);
                if (sk >= 0) {
                    const uint32_t r2 = g179a0::Roll(e, unit);
                    if ((int32_t)r2 < e.rd16s(row + 0x68 + d)) {
                        e.h(0xF2380, (X & ~0xFFu) | e.rd8(row + 0x180), unit, 0, game, 0, (uint32_t)sk, tgt, X, Y);
                        setCool();
                        return;
                    }
                }
                idleB();
                return;
            }
        }
        if ((int32_t)e.rd32(ctl + 0x1C) > 0) e.wr32(ctl + 0x1C, e.rd32(ctl + 0x1C) - 1);
        e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
        uint32_t flag = 0x64;
        if (unit) {
            const uint32_t ms = e.h(0x1320, e.rd32(unit + 4));
            if (ms && (e.rd8(ms + 0x104) & e.rd32(ga + 0x10))) flag = 0x65;
        }
        uint32_t blk[8] = { 4, unit, tgt, 0, 0, flag << 8, 0, 0 };
        e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(blk), 1);
        e.blk(0x75E10, blk, 8);
        return;
    }
    if (sk >= 0 && dist < 10 && (int32_t)e.rd32(ctl + 0x1C) < 1) {
        const uint32_t r = g179a0::Roll(e, unit);
        if ((int32_t)r < e.rd16s(row + 0x62 + d)) {
            e.h(0xF2380, ((uint32_t)sk & ~0xFFu) | e.rd8(row + 0x180), unit, 0, game, 0, (uint32_t)sk, tgt, X, Y);
            setCool();
            return;
        }
    }
    if ((int32_t)e.rd32(ctl + 0x1C) < 1) {
        const uint32_t r = g179a0::Roll(e, unit);
        if ((int32_t)r >= e.rd16s(row + 0x5C + d)) {
            if (dist <= 10) { idleB(); return; }
            e.h(0xF1D90, 6, tgt, 0, 0, 0, game, unit, 2, 9);
            return;
        }
    }
    e.h(0xF2540, 0, game, unit, 0, 0, tgt, 2, 0, 0);
}
}
