// 1.13c D2Game+0x7D910 (AI_ModeAttackStep; D2MOO AITHINK_Fn075_QuillMother): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_179a0.h). tp: +8 target, +0x18 mode, +0x1C monstats row. The unit's seed steps as in 0x179A0 (Roll).
// Row words indexed by game byte +0x6D: +0x56 / +0x5C attack chance (mode / no mode), +0x62 / +0x68 idle frames.
// A unit (type 1) whose data (+0x14) byte +0x54 is 3 or 0x13 calls 0x212F0 (EAX = unit, stack game, &block {2 words of stack residue, 1, target type (6 if none), target +0xC (-1 if none)}),
// then: mode != 0 runs 0xA496(unit, 0) and the command {4, unit, target, 0, 0, flag << 8, 0, 0} through 0x75E10 (flag 0x65 when the unit's monstats row (0x1320) byte +0x104 &
// global(0xF8BE0)[+0x10], else 0x64); mode 0 calls 0xF2540 (ECX = game, EDX = unit, stack target, 2, 0, 0, 1, 7). Any other unit rolls: mode != 0 and roll < word(+0x56) is the command, a
// miss idles (0xF22C0, EAX = game, ESI = unit, stack word(+0x62)); mode 0 and roll < word(+0x5C) is 0xF2540, else idle word(+0x68).
#pragma once
#include <cstdint>
#include "d2game_179a0.h"

namespace g7d910 {
template <class E> void Command(E& e, uint32_t game, uint32_t unit, uint32_t tgt, uint32_t ga)
{
    e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
    uint32_t flag = 0x64;
    if (unit) {
        const uint32_t ms = e.h(0x1320, e.rd32(unit + 4));
        if (ms && (e.rd8(ms + 0x104) & e.rd32(ga + 0x10))) flag = 0x65;
    }
    uint32_t blk[8] = { 4, unit, tgt, 0, 0, flag << 8, 0, 0 };
    e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(blk), 1);
    e.blk(0x75E10, blk, 8);
}
template <class E> void ModeAttackStep(E& e, uint32_t game, uint32_t unit, uint32_t tp, uint32_t ga)
{
    // 1.13c D2Game+0x7D910
    const uint32_t tgt = e.rd32(tp + 8), mode = e.rd32(tp + 0x18), row = e.rd32(tp + 0x1C);
    const uint32_t d = e.rd8(game + 0x6D) * 2;
    if (unit && e.rd32(unit) == 1) {
        const uint32_t pd = e.rd32(unit + 0x14);
        const uint32_t k = pd ? e.rd32(pd + 0x54) : 0;
        if (pd && (k == 3 || k == 0x13)) {
            uint32_t blk[6] = { 0, 0, 1, tgt ? e.rd32(tgt) : 6u, tgt ? e.rd32(tgt + 0xC) : 0xFFFFFFFFu, 0 };
            e.h(0x212F0, unit, 0, 0, 0, 0, game, e.stk(blk));
            e.blk(0x212F0, blk, 5, 0x1C);
            if (mode) { Command(e, game, unit, tgt, ga); return; }
            e.h(0xF2540, 0, game, unit, 0, 0, tgt, 2, 0, 0);
            return;
        }
    }
    const uint32_t r = g179a0::Roll(e, unit);
    if (mode) {
        if ((int32_t)r < e.rd16s(row + 0x56 + d)) { Command(e, game, unit, tgt, ga); return; }
        e.h(0xF22C0, game, 0, 0, 0, unit, (uint32_t)e.rd16s(row + 0x62 + d));
        return;
    }
    if ((int32_t)r < e.rd16s(row + 0x5C + d)) { e.h(0xF2540, 0, game, unit, 0, 0, tgt, 2, 0, 0); return; }
    e.h(0xF22C0, game, 0, 0, 0, unit, (uint32_t)e.rd16s(row + 0x68 + d));
}
}
