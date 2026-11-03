// 1.13c D2Game+0x179A0 (AITHINK_Fn124_FrozenHorror): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_177e0.h). tp: +8 target, +0x14 range, +0x18 mode, +0x1C monstats row. The unit's seed
// (+0x20 lo, +0x24 hi) steps new = lo * 0x6AC690C5 + hi (64 bit), roll = new lo % 100. Row words indexed by game byte +0x6D: +0x56/+0x5C chance, +0x62 cast chance,
// +0x68 idle frames; row +0x170 = skill (word), +0x180 = skill byte handed to 0xF2380 in AL.
// Level: skill >= 0 and 0xA3F4(ECX = unit, EDX = skill) != 0 gives 0xA400(unit, that, 1), negative clamps to 0. Cast needs skill >= 0, range < level, roll < row +0x62
// and 0xA68E(unit, 0xC) == 0: 0xF2380 (EAX = sext(skill) with low byte row +0x180, ECX = unit, EBX = game, stack = skill, target, 0, 0). Otherwise a non-zero
// 0xA68E(unit, 0xC) is cleared with 0xA3CA(unit, 0xC, 0); mode != 0: roll < row +0x56 runs 0xA496(unit, 0) and the command {4, unit, target, 0, 0, flag << 8, 0, 0}
// through 0x75E10 (stack = game, &block, 1), flag 0x65 when the unit's monstats row (0x1320) byte +0x104 & ga[+0x10] (global 0xF8BE0) else 0x64; else idle (0xF22C0,
// EAX = game, stack = row +0x68). Mode 0: roll < row +0x5C calls 0xF2540 (ECX = game, EDX = unit, stack = target, 2, 0, 0, 1, 0), else idle.
#pragma once
#include <cstdint>

namespace g179a0 {
template <class E> uint32_t Roll(E& e, uint32_t unit)
{
    const uint64_t n = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5u + e.rd32(unit + 0x24);
    e.wr32(unit + 0x20, (uint32_t)n); e.wr32(unit + 0x24, (uint32_t)(n >> 32));
    return (uint32_t)n % 100u;
}
template <class E> void FrozenHorror(E& e, uint32_t game, uint32_t unit, uint32_t tp, uint32_t ga)
{
    // 1.13c D2Game+0x179A0
    const uint32_t range = e.rd32(tp + 0x14), row = e.rd32(tp + 0x1C), mode = e.rd32(tp + 0x18), tgt = e.rd32(tp + 8);
    const uint32_t d = e.rd8(game + 0x6D) * 2;
    const int32_t sk = e.rd16s(row + 0x170);
    int32_t lvl = 0;
    if (sk >= 0) {
        const uint32_t sp = e.h(0xA3F4, 0, unit, (uint32_t)sk);
        if (sp) { lvl = (int32_t)e.h(0xA400, 0, 0, 0, 0, 0, unit, sp, 1); if (lvl < 0) lvl = 0; }
    }
    if (sk >= 0 && (int32_t)range < lvl) {
        const uint32_t r = Roll(e, unit);
        if ((int32_t)r < e.rd16s(row + 0x62 + d) && e.h(0xA68E, 0, 0, 0, 0, 0, unit, 0xC) == 0) {
            e.h(0xF2380, ((uint32_t)sk & ~0xFFu) | e.rd8(row + 0x180), unit, 0, game, 0, (uint32_t)sk, tgt, 0, 0);
            return;
        }
    }
    if (e.h(0xA68E, 0, 0, 0, 0, 0, unit, 0xC)) e.h(0xA3CA, 0, 0, 0, 0, 0, unit, 0xC, 0);
    if (mode) {
        const uint32_t r = Roll(e, unit);
        if ((int32_t)r < e.rd16s(row + 0x56 + d)) {
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
        e.h(0xF22C0, game, 0, 0, 0, unit, (uint32_t)e.rd16s(row + 0x68 + d));
        return;
    }
    const uint32_t r = Roll(e, unit);
    if ((int32_t)r < e.rd16s(row + 0x5C + d)) { e.h(0xF2540, 0, game, unit, 0, 0, tgt, 2, 0, 0); return; }
    e.h(0xF22C0, game, 0, 0, 0, unit, (uint32_t)e.rd16s(row + 0x68 + d));
}
}
