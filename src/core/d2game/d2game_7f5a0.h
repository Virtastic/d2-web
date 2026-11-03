// 1.13c D2Game+0x7F5A0 (AI_AttackStateMachine; D2MOO AITHINK_Fn046_ElementalBeast): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_179a0.h). tp: +0 ai general (state word +0x14), +8 target, +0x14 range, +0x18 mode, +0x1C monstats row.
// Row words indexed by game byte +0x6D: +0x56 approach chance, +0x5C activation distance, +0x62 idle frames. State 2: 0xDEEE0 (ECX = game, EDX = unit, stack target, 1).
// State 0: range < word(+0x5C) or 0xB0BE0(EAX = unit) (a dying-unit test) starts the attack: 0xA496(unit, 0), the command {8, unit, target, 0, 0, flag << 8, 0, 0}
// through 0x75E10 (flag 0x65 when the unit's monstats row (0x1320) byte +0x105 & *global 0xF8BE0, else 0x64), state := 1; otherwise idle. State 1: mode != 0 runs
// 0xF2220 (EAX = 8, EDI = unit, stack game, target), schedules 0x8EA80 (ECX = game, EDX = unit, stack 0, game frame (+0xA8) + 1, 0, 0, 0), state := 2; mode 0 rolls 0x1180
// (EAX = 100, ECX = &unit seed) against word(+0x56): below calls 0xF2540 (ECX = game, EDX = unit, stack target, 2, 0, 0, 1, 0), else 0xF1B10 (AL = 8 over the roll, stack game, unit).
// Any other state and a failed state 0 idle through 0xF22C0 (EAX = game, ESI = unit, stack word(+0x62)).
#pragma once
#include <cstdint>

namespace g7f5a0 {
template <class E> void AttackStateMachine(E& e, uint32_t game, uint32_t unit, uint32_t tp, uint32_t ga)
{
    // 1.13c D2Game+0x7F5A0
    const uint32_t ai = e.rd32(tp), tgt = e.rd32(tp + 8), range = e.rd32(tp + 0x14), mode = e.rd32(tp + 0x18), row = e.rd32(tp + 0x1C);
    const uint32_t st = e.rd32(ai + 0x14), d = e.rd8(game + 0x6D) * 2;
    if (st == 2) { e.h(0xDEEE0, 0, game, unit, 0, 0, tgt, 1); return; }
    if (st == 0) {
        if ((int32_t)range < e.rd16s(row + 0x5C + d) || e.h(0xB0BE0, unit)) {
            e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
            uint32_t flag = 0x64;
            if (unit) {
                const uint32_t ms = e.h(0x1320, e.rd32(unit + 4));
                if (ms && (e.rd8(ms + 0x105) & e.rd32(ga))) flag = 0x65;
            }
            uint32_t blk[8] = { 8, unit, tgt, 0, 0, flag << 8, 0, 0 };
            e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(blk), 1);
            e.blk(0x75E10, blk, 8);
            e.wr32(ai + 0x14, 1);
            return;
        }
    } else if (st == 1) {
        if (mode == 0) {
            const uint32_t r = e.h(0x1180, 0x64, unit + 0x20);
            if ((int32_t)r < e.rd16s(row + 0x56 + d)) { e.h(0xF2540, 0, game, unit, 0, 0, tgt, 2, 0, 0); return; }
            e.h(0xF1B10, (r & ~0xFFu) | 8, 0, 0, 0, 0, game, unit);
            return;
        }
        e.hj(0xF2220, 8, 0, 0, 0, 0, unit, game, tgt);
        e.h(0x8EA80, 0, game, unit, 0, 0, 0, e.rd32(game + 0xA8) + 1, 0, 0);
        e.wr32(ai + 0x14, 2);
        return;
    }
    e.h(0xF22C0, game, 0, 0, 0, unit, (uint32_t)e.rd16s(row + 0x62 + d));
}
}
