// 1.13c D2Game+0x1C8A0 (monster ranged-or-chooser AI helper): EAX = pUnit, stack = game, class, a2, a3, pSeed, tp (ret 0x18). Written from the 1.13c disassembly as a
// rule over E (see d2game_17cb0.h). C = tp[0] (C+0x14 is a counter), row = tp+0x1C; w = row short +0x56 indexed by the game byte +0x6D (as words).
// Chance edi: 0x62 for class 0x152 and 0x230..0x231, else min(C[0x14] + 0x28 + 2 * 0xA42A (unit, 0xC, 0), 0x5F). d = 0xB0A20 (a3); k = 0x96570 (EAX = a2) == 0 idles
// (EAX = game, ESI = unit, 0xA); K = k[2]. A seed step; roll r = lo % 100: flag = r < edi; flag clears C[0x14] to 0, else adds 10.
// w == 0: d < 4 and a second roll < 0x32: 0xF2870 (EAX = quotient over 4, ECX = a2, stack game, unit) != 0 ends; else every attached unit state of type 2 (unit+0xDC list, +0x24
// next) gets 0x8E6F0 (EAX = node); 0xF2D80 (EAX = a3, stack game, unit, 4, 1) != 0 ends; else the chooser. Otherwise (d >= 4 or roll high) flag clear idles, flag set the chooser.
// w != 0: d < 3 and 0xA754 (unit, a3, 0) != 0: flag clear idles, set the chooser. Else 0xAD18 (unit+0x2C, byte tp[0x20]+0xE) and 0xF2540 (ECX = game, EDX = unit, stack a3,
// 0xF, 0, 0, 1, 1). The chooser is 0x1C4E0 (EAX = unit, stack game, class, K, a3, seed; a hook here).
#pragma once
#include <cstdint>

namespace g1c8a0 {
template <class E> void Choose(E& e, uint32_t unit, uint32_t game, uint32_t cls, uint32_t a2, uint32_t a3, uint32_t S, uint32_t tp)
{
    // 1.13c D2Game+0x1C8A0
    const uint32_t row = e.rd32(tp + 0x1C), C = e.rd32(tp);
    auto idle = [&]() { e.h(0xF22C0, game, 0, 0, 0, unit, 0xA); };
    int32_t edi;
    if (cls == 0x152 || (cls > 0x22F && cls <= 0x231)) edi = 0x62;
    else {
        const int32_t lvl = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, unit, 0xC, 0);
        edi = (int32_t)e.rd32(C + 0x14) + 0x28 + lvl * 2;
        if (edi >= 0x5F) edi = 0x5F;
    }
    const int32_t d = (int32_t)e.h(0xB0A20, 0, 0, 0, 0, 0, a3);
    const uint32_t k = e.h(0x96570, a2);
    if (!k) { idle(); return; }
    const uint32_t K = e.rd32(k + 8);
    uint64_t v = (uint64_t)e.rd32(S) * 0x6AC690C5u + e.rd32(S + 4);
    e.wr32(S + 4, (uint32_t)(v >> 32)); e.wr32(S, (uint32_t)v);
    const bool flag = (int32_t)((uint32_t)v % 100) < edi;
    e.wr32(C + 0x14, flag ? 0 : e.rd32(C + 0x14) + 10);
    auto chooser = [&]() { e.h(0x1C4E0, unit, 0, 0, 0, 0, game, cls, K, a3); };
    const uint32_t w = (uint32_t)e.rd16s(row + 0x56 + e.rd8(game + 0x6D) * 2) & 0xFFFF;
    if (w == 0) {
        if (d < 4) {
            v = (uint64_t)e.rd32(S) * 0x6AC690C5u + e.rd32(S + 4);
            e.wr32(S + 4, (uint32_t)(v >> 32)); e.wr32(S, (uint32_t)v);
            if ((uint32_t)v % 100 < 0x32) {
                if (e.h(0xF2870, (((uint32_t)v / 100) & ~0xFFu) | 4, 0, 0, 0, 0, game, unit)) return;
                if (unit)
                    for (uint32_t n = e.rd32(unit + 0xDC); n;) { const uint32_t nx = e.rd32(n + 0x24); if (e.rd8(n) == 2) e.h(0x8E6F0, n); n = nx; }
                if (e.h(0xF2D80, a3, 0, 0, 0, 0, game, unit, 4, 1)) return;
                chooser(); return;
            }
        }
        if (!flag) { idle(); return; }
        chooser(); return;
    }
    if (d < 3 && e.h(0xA754, 0, 0, 0, 0, 0, unit, a3, 0)) {
        if (!flag) { idle(); return; }
        chooser(); return;
    }
    e.h(0xAD18, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C), e.rd8(e.rd32(tp + 0x20) + 0xE));
    e.h(0xF2540, 0, game, unit, 0, 0, a3, 0xF, 0, 0, 1, 1);
}
}
