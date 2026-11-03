// 1.13c D2Game+0x1B780 (AITHINK_Fn095_PantherJavelin): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_17cb0.h). tp: +8 target, +0x14 range, +0x1C monstats row; wNN = row short +NN
// indexed by the game byte +0x6D; the seed roll is the unit's +0x20/+0x24 step mod 100.
// 0xB1BC0 (EAX = unit, ECX = game, stack = &range, ret 4) returns a ranged target and rewrites the range local (initially tp+0x14).
// Range < 8 and roll < w68: 0xF2D80 (EAX = target, stack = game, unit, 0x10, 1). Else range > w74 - 6 and roll < w56: 0xF2870 (EAX = 4,
// ECX = target, stack = game, unit). Else a B1BC0 result with range < w74: roll < w5C sends 0xF2220 (EAX = 4, EDI = ESI = unit, stack = game, result),
// else idle 0xF22C0 (EAX = game, ESI = unit, stack = w6E). Otherwise 0xB17B0 (EAX = 1, ECX = &{0, 0x7FFFFFFF}, EDX = game, stack = unit, 0x6FCBCA00)
// fills {b0, b1}: b0 set and w62 < b1 attacks with 0xF2540 (ECX = game, EDX = unit, stack = b0, 2, 0, 0, 1, 7), else idle with w6E.
#pragma once
#include <cstdint>
#include "d2game_17cb0.h"

namespace g1b780 {
template <class E> void PantherJavelin(E& e, uint32_t game, uint32_t unit, uint32_t tp)
{
    // 1.13c D2Game+0x1B780
    const uint32_t row = e.rd32(tp + 0x1C), tgt = e.rd32(tp + 8);
    uint32_t dist = e.rd32(tp + 0x14);
    auto w = [&](uint32_t off) { return e.rd16s(row + off + e.rd8(game + 0x6D) * 2); };
    auto idle = [&]() { e.h(0xF22C0, game, 0, 0, 0, unit, (uint32_t)w(0x6E)); };
    const uint32_t res = e.h(0xB1BC0, unit, game, 0, 0, 0, e.stk(&dist));
    if ((int32_t)dist < 8 && (int32_t)g17cb0::Roll(e, unit) < w(0x68)) { e.h(0xF2D80, tgt, 0, 0, 0, 0, game, unit, 0x10, 1); return; }
    if (w(0x74) - 6 < (int32_t)dist && (int32_t)g17cb0::Roll(e, unit) < w(0x56)) { e.h(0xF2870, 4, tgt, 0, 0, 0, game, unit); return; }
    if (res && (int32_t)dist < w(0x74)) {
        if ((int32_t)g17cb0::Roll(e, unit) < w(0x5C)) { e.h(0xF2220, 4, 0, 0, 0, unit, game, res); e.j(0xF2220, unit); return; }
        idle(); return;
    }
    uint32_t blk[2] = { 0, 0x7FFFFFFFu };
    const uint32_t in[2] = { 0, 0x7FFFFFFFu };
    e.h(0xB17B0, 1, e.stk(blk), game, 0, 0, unit, 0x6FCBCA00u);
    e.blk(0xB17B0, in, 2, 0x3);   // retail logs the block at hook entry, before the hook's out-words land
    if (blk[0] && w(0x62) < (int32_t)blk[1]) { e.h(0xF2540, 0, game, unit, 0, 0, blk[0], 2, 0, 0, 1, 7); return; }
    idle();
}
}
