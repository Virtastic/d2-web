// 1.13c D2Game+0xA6530 (AI think callback, attack / approach): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_177e0.h). tp: +8 target, +0x14 range, +0x18 mode, +0x1C monstats row. Row words indexed by game byte
// +0x6D: +0x56 / +0x5C / +0x68 chances, +0x62 range, +0x170 skill, +0x180 skill byte. The unit seed steps new = lo * 0x6AC690C5 + hi, roll = new lo % 100.
// Mode != 0: roll < +0x56 sends 0xA496(unit, 0) and the command {4, unit, target, 0, 0, flag << 8, 0, 0} through 0x75E10 (stack = game, &block, 1), flag 0x65 when the
// unit's monstats row (0x1320, EAX = unit +4) byte +0x104 & ga[+0x10] (global 0xF8BE0), else 0x64; roll >= +0x56 idles 0xF22C0 (EAX = game, ESI = unit, stack = 0xF).
// Mode 0: skill >= 0 and range < +0x62 and a roll < +0x68 casts 0xF2380 (EAX = skill with the low byte row +0x180, ECX = unit, EBX = game, stack = skill, target, 0, 0);
// else a roll < +0x5C calls 0xF2540 (ECX = game, EDX = unit, stack = target, 2, 0, 0, 1, 0); else idle 0xF.
#pragma once
#include <cstdint>

namespace ga6530 {
template <class E> uint32_t Roll(E& e, uint32_t unit)
{
    const uint64_t n = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5u + e.rd32(unit + 0x24);
    e.wr32(unit + 0x20, (uint32_t)n); e.wr32(unit + 0x24, (uint32_t)(n >> 32));
    return (uint32_t)n % 100u;
}
template <class E> void Think(E& e, uint32_t game, uint32_t unit, uint32_t tp, uint32_t ga)
{
    // 1.13c D2Game+0xA6530
    const uint32_t tgt = e.rd32(tp + 8), mode = e.rd32(tp + 0x18), row = e.rd32(tp + 0x1C);
    const int32_t range = (int32_t)e.rd32(tp + 0x14);
    const uint32_t d = e.rd8(game + 0x6D) * 2;
    auto w = [&](uint32_t o) { return e.rd16s(row + o + d); };
    if (mode) {
        if ((int32_t)Roll(e, unit) < w(0x56)) {
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
    } else {
        if (e.rd16s(row + 0x170) >= 0 && range < w(0x62) && (int32_t)Roll(e, unit) < w(0x68)) {
            const uint32_t sk = (uint32_t)e.rd16s(row + 0x170);
            e.h(0xF2380, (sk & ~0xFFu) | e.rd8(row + 0x180), unit, 0, game, 0, sk, tgt, 0, 0);
            return;
        }
        if ((int32_t)Roll(e, unit) < w(0x5C)) { e.h(0xF2540, 0, game, unit, 0, 0, tgt, 2, 0, 0, 1, 0); return; }
    }
    e.h(0xF22C0, game, 0, 0, 0, unit, 0xF);
}
}
