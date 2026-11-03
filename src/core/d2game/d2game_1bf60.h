// 1.13c D2Game+0x1BF60 (AITHINK_Fn031_NpcOutOfTown): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_17cb0.h). f = the unit's class (+4) is 0x20F. pd = the unit's data (+0x14, unit type 1 only).
// 0x1BDF0 (EAX = unit, stack = game) != 0 ends. f: 0x59F80 (EAX = game). Walk pd+0x30's list (head word, node +8 = next): a node with word 0 set idles
// 0xF22C0 (EAX = game, ESI = unit, stack = 0x28). Find node q in the circular list at (pd+0x28)+0x20 whose word +8 is 3 (starting at the first node, ending at
// the list head itself). Unit mode (+0x10) 0xC ends. 0x19C20 (EAX = unit, stack = game, tp). No q: 0x1BDF0 again, idle 0x28.
// q+0x14 >= 2: it is incremented; step = f ? 0x56A40 : 0x60450 (ECX = game, EDX = unit, stack = &{w0, w1}, ret 4) returns 0, idle 0x14. If q+0x14 < 8 and
// 0xB07B0 (EAX = unit, stack = w0, w1) != 0: 0xF2540 (ECX = game, EDX = unit, stack = 0, 2, w0, w1, 1, 0). Else fn2 = f ? 0x5CC30 : 0x663A0 (ECX = game, EDX = unit),
// 0xA472 (import thunk: unit, 6, 0, 0), then 0xF21E0 (EAX = 0xC, EDI = unit, stack = game, 0x1100 X, 0x1130 Y; both ECX = unit).
// q+0x14 < 2: 0xB07B0 (EAX = unit, stack = q+0xC, q+0x10) > 1 and q+0x18 <= 5: q+0x18++, f and 0x56950 (EAX = game) != 0 idles 0x14 and clears q+0x18, else
// 0xF2540 (.., 0, 2, q+0xC, q+0x10, 1, 0). Otherwise q+0x14 == 1: probe = f ? 0x57CB0 : 0x61340 (ECX = game, EDX = unit); 0 sets q+0x14 = q+0x18 = 1, else q+0x14 = 2;
// then idle 0x14.
#pragma once
#include <cstdint>

namespace g1bf60 {
template <class E> void NpcOutOfTown(E& e, uint32_t game, uint32_t unit, uint32_t tp)
{
    // 1.13c D2Game+0x1BF60
    const bool f = unit && e.rd32(unit + 4) == 0x20F;
    auto idle = [&](uint32_t n) { e.h(0xF22C0, game, 0, 0, 0, unit, n); };
    if (e.h(0x1BDF0, unit, 0, 0, 0, 0, game)) return;
    if (f) e.h(0x59F80, game);
    const uint32_t pd = (unit && e.rd32(unit) == 1) ? e.rd32(unit + 0x14) : 0;
    const uint32_t p = pd ? e.rd32(pd + 0x30) : 0;
    for (uint32_t n = e.rd32(p); n; n = e.rd32(n + 8))
        if (e.rd32(n)) { idle(0x28); return; }
    uint32_t q = 0;
    if (pd) {
        const uint32_t x = e.rd32(pd + 0x28);
        const uint32_t L = x ? e.rd32(x + 0x20) : 0;
        if (L) {
            uint32_t c = e.rd32(L);
            if (e.rd32(c + 8) != 3) while (c != L) { c = e.rd32(c); if (e.rd32(c + 8) == 3) break; }
            if (e.rd32(c + 8) == 3) q = c;
        }
    }
    if (unit && e.rd32(unit + 0x10) == 0xC) return;
    e.h(0x19C20, unit, 0, 0, 0, 0, game, tp);
    if (!q) { e.h(0x1BDF0, unit, 0, 0, 0, 0, game); idle(0x28); return; }
    const int32_t n5 = (int32_t)e.rd32(q + 0x14);
    if (n5 >= 2) {
        e.wr32(q + 0x14, (uint32_t)n5 + 1);
        uint32_t lw[2] = { 0, 0 };
        if (!e.h(f ? 0x56A40 : 0x60450, 0, game, unit, 0, 0, e.stk(lw))) { idle(0x14); return; }
        if ((int32_t)e.rd32(q + 0x14) < 8 && e.h(0xB07B0, unit, 0, 0, 0, 0, lw[0], lw[1])) {
            e.h(0xF2540, 0, game, unit, 0, 0, 0, 2, lw[0], lw[1], 1, 0);
            return;
        }
        e.h(f ? 0x5CC30 : 0x663A0, 0, game, unit);
        e.h(0xA472, 0, 0, 0, 0, 0, unit, 6, 0, 0);
        const uint32_t y = e.h(0x1130, 0, unit);
        const uint32_t x = e.h(0x1100, 0, unit);
        e.h(0xF21E0, 0xC, 0, 0, 0, 0, game, x, y);
        e.j(0xF21E0, unit);
        return;
    }
    const int32_t c = (int32_t)e.h(0xB07B0, unit, 0, 0, 0, 0, e.rd32(q + 0xC), e.rd32(q + 0x10));
    if (c > 1 && (int32_t)e.rd32(q + 0x18) <= 5) {
        e.wr32(q + 0x18, e.rd32(q + 0x18) + 1);
        if (f && e.h(0x56950, game)) { idle(0x14); e.wr32(q + 0x18, 0); return; }
        e.h(0xF2540, 0, game, unit, 0, 0, 0, 2, e.rd32(q + 0xC), e.rd32(q + 0x10), 1, 0);
        return;
    }
    if (n5 == 1) {
        if (!e.h(f ? 0x57CB0 : 0x61340, 0, game, unit)) { e.wr32(q + 0x14, 1); e.wr32(q + 0x18, 1); idle(0x14); return; }
        e.wr32(q + 0x14, 2);
    }
    idle(0x14);
}
}
