// 1.13c D2Game+0xA9D40 (sub_A9D40; D2MOO AITHINK_Fn136_BaalTaunt): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_179a0.h). tp: +0 ai general (counter word +0x14), +8 target, +0x14 range, +0x1C monstats row. Row words indexed
// by game byte +0x6D: +0x56 / +0x62 range limits, +0x5C counter limit. No target idles 0x19 (0xF22C0: EAX = game, ESI = unit, stack 0x19). A target of type 0 or 1 whose +0x10 is 1
// bumps the counter and compares it with word(+0x5C); every other case clears it. Counter over the limit casts: 0xF2380 (EAX = ai pointer with AL = 4, ECX = unit, EBX = game,
// stack 0x11C, target, 0, 0). Otherwise range > word(+0x62) asks 0xE19C0 (EAX = 0x1130(target), ECX = 0x1100(target), EDX = 0xA478(target), stack game, unit, 0, 0; ret 0x10): non-zero
// idles 0x19. Then range > word(+0x56) calls 0xF2540 (ECX = game, EDX = unit, stack target, 2, 0, 0, 1, 0), else idle 0x19.
#pragma once
#include <cstdint>

namespace ga9d40 {
template <class E> void Taunt(E& e, uint32_t game, uint32_t unit, uint32_t tp)
{
    // 1.13c D2Game+0xA9D40
    const uint32_t ai = e.rd32(tp), tgt = e.rd32(tp + 8), range = e.rd32(tp + 0x14), row = e.rd32(tp + 0x1C);
    const uint32_t d = e.rd8(game + 0x6D) * 2;
    if (!tgt) { e.h(0xF22C0, game, 0, 0, 0, unit, 0x19); return; }
    const uint32_t ty = e.rd32(tgt);
    const bool cond = (ty == 0 || ty == 1) && e.rd32(tgt + 0x10) == 1;
    bool over = false;
    if (cond) {
        const uint32_t st = e.rd32(ai + 0x14) + 1;
        e.wr32(ai + 0x14, st);
        over = (int32_t)st > e.rd16s(row + 0x5C + d);
    }
    if (!cond || over) {
        e.wr32(ai + 0x14, 0);
        if (over) { e.h(0xF2380, (ai & ~0xFFu) | 4, unit, 0, game, 0, 0x11C, tgt, 0, 0); return; }
    }
    if ((int32_t)range > e.rd16s(row + 0x62 + d)) {
        const uint32_t y = e.h(0x1130, 0, tgt), x = e.h(0x1100, 0, tgt), t = e.h(0xA478, 0, 0, 0, 0, 0, tgt);
        if (e.h(0xE19C0, y, x, t, 0, 0, game, unit, 0, 0)) { e.h(0xF22C0, game, 0, 0, 0, unit, 0x19); return; }
    }
    if ((int32_t)range > e.rd16s(row + 0x56 + d)) { e.h(0xF2540, 0, game, unit, 0, 0, tgt, 2, 0, 0); return; }
    e.h(0xF22C0, game, 0, 0, 0, unit, 0x19);
}
}
