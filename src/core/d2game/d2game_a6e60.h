// 1.13c D2Game+0xA6E60 (AIApproachMeleeTargetState): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_179a0.h). tp+0 = ai general (state +0x14, counter +0x18, +0x1C), tp+8 = target, tp+0x14 = range.
// 0xE2780 (EAX = game, EBX = unit, ESI = &{x, y}) fills the approach point and returns non-zero when there is one; a non-zero result with ai state 0 sets
// state 1 and clears +0x18 / +0x1C. No point or no target idles 0xF22C0 (EAX = game, ESI = unit, stack 0xA). A signed range of 0x14 or more idles 0x28.
// State 1: 0xF2540 (ECX = game, EDX = unit, stack 0, 2, x, y, 1, 0), state := 2. State 2: 0xB07B0 (EAX = unit, stack x, y); a result of 2 or more with the
// counter below 3 increments it, sets unit data +0x2C -> +0x18 to 7 and walks again through 0xF2540; otherwise unit +0xC4 |= 0x20000, 0xF21E0 (EAX = 0,
// stack game, 0, 0), then 0xE6760 (EAX = game, EDX = unit). Any other state idles 0x28.
#pragma once
#include <cstdint>

namespace ga6e60 {
template <class E> void Approach(E& e, uint32_t game, uint32_t unit, uint32_t tp)
{
    // 1.13c D2Game+0xA6E60
    const uint32_t ai = e.rd32(tp), tgt = e.rd32(tp + 8);
    const int32_t range = (int32_t)e.rd32(tp + 0x14);
    uint32_t loc[2] = { 0, 0 };
    const uint32_t r = e.h(0xE2780, game, 0, 0, unit, e.stk(loc));
    if (r && e.rd32(ai + 0x14) == 0) { e.wr32(ai + 0x14, 1); e.wr32(ai + 0x18, 0); e.wr32(ai + 0x1C, 0); }
    if (!r || !tgt) { e.h(0xF22C0, game, 0, 0, 0, unit, 0xA); return; }
    if (range >= 0x14) { e.h(0xF22C0, game, 0, 0, 0, unit, 0x28); return; }
    const uint32_t st = e.rd32(ai + 0x14);
    if (st == 1) {
        e.h(0xF2540, 0, game, unit, 0, 0, 0, 2, loc[0], loc[1]);
        e.wr32(ai + 0x14, 2);
        return;
    }
    if (st != 2) { e.h(0xF22C0, game, 0, 0, 0, unit, 0x28); return; }
    const uint32_t b = e.h(0xB07B0, unit, 0, 0, 0, 0, loc[0], loc[1]);
    if (b >= 2 && (int32_t)e.rd32(ai + 0x18) < 3) {
        e.wr32(ai + 0x18, e.rd32(ai + 0x18) + 1);
        uint32_t sd = 0;
        if (unit && e.rd32(unit) == 1) { const uint32_t pd = e.rd32(unit + 0x14); if (pd) sd = e.rd32(pd + 0x2C); }
        if (sd) e.wr32(sd + 0x18, 7);   // retail writes through the pointer unchecked (a null one faults, so no vector reaches it)
        e.h(0xF2540, 0, game, unit, 0, 0, 0, 2, loc[0], loc[1]);
        return;
    }
    if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x20000);
    e.h(0xF21E0, 0, 0, 0, 0, 0, game, 0, 0);
    e.h(0xE6760, game, 0, unit);
}
}
