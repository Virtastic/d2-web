// 1.13c D2Game+0x6D620 (ObjectOperateQuestToggle think entry): ecx = think context {+0 game, +4 unit, +8 owner, +0x10 monster id}, plain ret, returns 0.
// Rule: t = 0xA556(0xA478(unit)). A null unit or one with +0x10 == 0: mode 1 (0xA4A2), record of the id (0xA520), mode-1 frame send (0x8EA80).
// Otherwise t == 0x4A: 0x9E170 (eax = owner; stack game, 0) then the first node of game+0x10F4's list whose +0 is 0xB (chain +0xF4); its
// record +0x18 gets word +0x44 = 1 and word +0x42 = 2 when word +0x44 was 0. t == 0x36: the same with 0x9E170 (game, 0xB) and the
// words swapped (+0x42 = 1, +0x44 = 2 when +0x42 was 0). Any other t: nothing.
#pragma once
#include <cstdint>

namespace r6d620 {
template <class E> uint32_t Toggle(E& e, uint32_t ctx)
{
    const uint32_t game = e.rd32(ctx);
    const uint32_t t = e.h(0xA556, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, e.rd32(ctx + 4)));
    const uint32_t unit = e.rd32(ctx + 4);
    if (!unit || !e.rd32(unit + 0x10)) {
        e.h(0xA4A2, 0, 0, 0, 0, 0, unit, 1);
        const uint32_t rec = e.h(0xA520, 0, 0, 0, 0, 0, e.rd32(ctx + 0x10));
        e.h(0x8EA80, 0, e.rd32(ctx), e.rd32(ctx + 4), 0, 0, 1, (e.rd32(rec + 0xDC) >> 8) + 1 + e.rd32(game + 0xA8), 0, 0, 0);
        return 0;
    }
    if (t != 0x4A && t != 0x36) return 0;
    const bool a = t == 0x4A;
    e.h(0x9E170, e.rd32(ctx + 8), 0, 0, 0, 0, game, a ? 0u : 0xBu);
    const uint32_t head = e.rd32(game + 0x10F4);
    if (!head) return 0;
    uint32_t n = e.rd32(head);
    while (n && e.rd32(n) != 0xB) n = e.rd32(n + 0xF4);
    if (!n) return 0;
    const uint32_t rec = e.rd32(n + 0x18);
    const uint32_t p = rec + (a ? 0x44 : 0x42), q = rec + (a ? 0x42 : 0x44);
    if (e.rd8(p) == 0 && e.rd8(p + 1) == 0) {
        e.wr8(p, 1); e.wr8(p + 1, 0);
        e.wr8(q, 2); e.wr8(q + 1, 0);
    }
    return 0;
}
}
