// 1.13c D2Game+0xA59B0 (UnitClearStateAndDeselect): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_179a0.h). tp+0 = ai general.
// Find(): a unit of type 1 whose data (+0x14), then +0x28, then +0x20 is a circular list (head word 0 = first node,
// node word 0 = next, +8 = kind, +0xC / +0x10 = payload) walks from the first node to the first node of kind 0xA
// (the walk stops after the head); none = 0.
// Order: V = node +0xC and W = node +0x10 of the found node (0 when none); a non-zero 0xA68E(unit, 0xC) is cleared with
// 0xA3CA(unit, 0xC, 0); 0x9D230 (ECX = game, EDX = unit, stack ai, ai word 0); find again: when that finds nothing and
// V and W are both non-zero, 0x21290 (EAX = game, ECX = unit, stack &{2 words of residue, 0xA, V, W}); finally idle
// 0xF22C0 (EAX = game, ESI = unit, stack 1).
#pragma once
#include <cstdint>

namespace ga59b0 {
template <class E> uint32_t Find(E& e, uint32_t unit)
{
    if (!unit || e.rd32(unit) != 1) return 0;
    const uint32_t pd = e.rd32(unit + 0x14);
    if (!pd) return 0;
    const uint32_t p2 = e.rd32(pd + 0x28);
    if (!p2) return 0;
    const uint32_t head = e.rd32(p2 + 0x20);
    if (!head) return 0;
    uint32_t n = e.rd32(head);
    for (;;) {
        if (e.rd32(n + 8) == 0xA) break;
        if (n == head) break;
        n = e.rd32(n);
    }
    return e.rd32(n + 8) == 0xA ? n : 0;
}
template <class E> void ClearStateAndDeselect(E& e, uint32_t game, uint32_t unit, uint32_t tp)
{
    // 1.13c D2Game+0xA59B0
    const uint32_t ai = e.rd32(tp);
    uint32_t v = 0, w = 0;
    if (const uint32_t n = Find(e, unit)) { v = e.rd32(n + 0xC); w = e.rd32(n + 0x10); }
    if (e.h(0xA68E, 0, 0, 0, 0, 0, unit, 0xC)) e.h(0xA3CA, 0, 0, 0, 0, 0, unit, 0xC, 0);
    e.h(0x9D230, 0, game, unit, 0, 0, ai, e.rd32(ai));
    if (!Find(e, unit) && v && w) {
        uint32_t blk[5] = { 0, 0, 0xA, v, w };
        e.h(0x21290, game, unit, 0, 0, 0, e.stk(blk));
        e.blk(0x21290, blk, 5, 0x1C);
    }
    e.h(0xF22C0, game, 0, 0, 0, unit, 1);
}
}
