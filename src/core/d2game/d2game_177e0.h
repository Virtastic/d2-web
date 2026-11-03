// 1.13c D2Game+0x177E0 (AITHINK_Fn125_BloodLord): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_15e00.h). tp: +8 target, +0x18 mode, +0x1C monstats row. The unit's seed (+0x20 lo, +0x24 hi)
// steps new = lo * 0x6AC690C5 + hi (64 bit), roll = new lo % 100. Row words indexed by game byte +0x6D: +0x56/+0x5C chance, +0x62 chance 2, +0x68 idle
// frames, +0x170 skill.
// Mode != 0: roll1 >= row +0x56 idles (0xF22C0, EAX = game, ESI = unit, stack = row +0x68). Else roll2 < row +0x62 casts 0xF2380 (EAX = skill with the low
// byte 5, ECX = unit, EBX = game, stack = skill, target, 0, 0). Else 0xA496(unit, 0) and the command {4, unit, target, 0, 0, flag << 8, 0, 0} through
// 0x75E10 (stack = game, &block, 1), flag 0x65 when the unit's monstats row (0x1320) byte +0x104 & ga[+0x10] (global 0xF8BE0) else 0x64.
// Mode 0: roll < row +0x5C calls 0xF2540 (ECX = game, EDX = unit, stack = target, 2, 0, 0, 1, 0). Else idle with row +0x68.
#pragma once
#include <cstdint>

namespace g177e0 {
template <class E> uint32_t Roll(E& e, uint32_t unit)
{
    const uint64_t n = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5u + e.rd32(unit + 0x24);
    e.wr32(unit + 0x20, (uint32_t)n); e.wr32(unit + 0x24, (uint32_t)(n >> 32));
    return (uint32_t)n % 100u;
}
template <class E> void BloodLord(E& e, uint32_t game, uint32_t unit, uint32_t tp, uint32_t ga)
{
    // 1.13c D2Game+0x177E0
    const uint32_t mode = e.rd32(tp + 0x18), row = e.rd32(tp + 0x1C), tgt = e.rd32(tp + 8);
    const uint32_t d = e.rd8(game + 0x6D) * 2;
    if (mode) {
        const uint32_t r1 = Roll(e, unit);
        if ((int32_t)r1 >= (int32_t)e.rd16s(row + 0x56 + d)) { e.h(0xF22C0, game, 0, 0, 0, unit, (uint32_t)e.rd16s(row + 0x68 + d)); return; }
        const uint32_t r2 = Roll(e, unit);
        if ((int32_t)r2 < (int32_t)e.rd16s(row + 0x62 + d)) {
            const uint32_t sk = (uint32_t)e.rd16s(row + 0x170);
            e.h(0xF2380, (sk & ~0xFFu) | 5, unit, 0, game, 0, sk, tgt, 0, 0);
            return;
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
        return;
    }
    const uint32_t r = Roll(e, unit);
    if ((int32_t)r < (int32_t)e.rd16s(row + 0x5C + d)) { e.h(0xF2540, 0, game, unit, 0, 0, tgt, 2, 0, 0); return; }
    e.h(0xF22C0, game, 0, 0, 0, unit, (uint32_t)e.rd16s(row + 0x68 + d));
}
}
