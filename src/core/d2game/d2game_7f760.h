// 1.13c D2Game+0x7F760 (sub_7F760, an AITHINK handler): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_179a0.h). tp: +0 ai general (state word +0x14), +8 target, +0x18 mode, +0x1C monstats row. The unit's seed
// steps as in 0x179A0 (Roll). Row words indexed by game byte +0x6D: +0x56 / +0x5C / +0x62 / +0x68 / +0x6E / +0x74 chances, row +0x170 / +0x172 = two skills (words),
// +0x180 / +0x181 their skill bytes (AL of 0xF2380, which also gets ECX = unit, EBX = game, stack skill, target, 0, 0).
// Mode != 0: a non-zero state rolls word(+0x56): a miss idles (0xF22C0, EAX = game, stack word(+0x5C)); state := 1; skill 1 (>= 0) rolls word(+0x6E) and casts; skill 2 (>= 0)
// rolls word(+0x74) and casts; else 0xA496(unit, 0) and the command {4, unit, target, 0, 0, flag << 8, 0, 0} through 0x75E10 (flag 0x65 when the unit's monstats row (0x1320)
// byte +0x104 & global(0xF8BE0)[+0x10], else 0x64). Mode 0: state := 0, roll word(+0x62): below calls 0xF2540 (ECX = game, EDX = unit, stack target, 2, 0, 0, 1, 7), else idle word(+0x68).
#pragma once
#include <cstdint>
#include "d2game_179a0.h"

namespace g7f760 {
template <class E> void Handler(E& e, uint32_t game, uint32_t unit, uint32_t tp, uint32_t ga)
{
    // 1.13c D2Game+0x7F760
    const uint32_t ai = e.rd32(tp), tgt = e.rd32(tp + 8), mode = e.rd32(tp + 0x18), row = e.rd32(tp + 0x1C);
    const uint32_t d = e.rd8(game + 0x6D) * 2;
    if (mode == 0) {
        e.wr32(ai + 0x14, 0);
        const uint32_t r = g179a0::Roll(e, unit);
        if ((int32_t)r < e.rd16s(row + 0x62 + d)) { e.h(0xF2540, 0, game, unit, 0, 0, tgt, 2, 0, 0); return; }
        e.h(0xF22C0, game, 0, 0, 0, unit, (uint32_t)e.rd16s(row + 0x68 + d));
        return;
    }
    if (e.rd32(ai + 0x14) != 0) {
        const uint32_t r = g179a0::Roll(e, unit);
        if ((int32_t)r >= e.rd16s(row + 0x56 + d)) { e.h(0xF22C0, game, 0, 0, 0, unit, (uint32_t)e.rd16s(row + 0x5C + d)); return; }
    }
    e.wr32(ai + 0x14, 1);
    static const uint32_t kSk[2] = { 0x170, 0x172 }, kCh[2] = { 0x6E, 0x74 }, kBy[2] = { 0x180, 0x181 };
    for (int i = 0; i < 2; i++) {
        const int32_t sk = e.rd16s(row + kSk[i]);
        if (sk < 0) continue;
        const uint32_t r = g179a0::Roll(e, unit);
        if ((int32_t)r < e.rd16s(row + kCh[i] + d)) {
            e.h(0xF2380, ((uint32_t)sk & ~0xFFu) | e.rd8(row + kBy[i]), unit, 0, game, 0, (uint32_t)sk, tgt, 0, 0);
            return;
        }
    }
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
}
