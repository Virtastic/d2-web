// 1.13c D2Game+0xE4AB0, transcribed from the 1.13c disassembly: the Act 1 quest (group 0x12) state step for a quest node.
// Convention: ECX = node (this), EDX = player unit, stack a1 (unused), qs (quest flag word), out (byte pointer); ret 0xC. Quest bits through 0xA568 (qs, group, bit);
// 0xE660 (EAX = unit, EBX = node +4, stack code) finds an item; 0xE2960 (EAX = game = node +4) is the quest-node flag lookup (AL). Writes the state byte; AL = 1,
// the upper 24 bits of EAX as the disassembly leaves them (the 0xE2960 result, a negated 0xA568 result, or the out pointer).
#pragma once
#include <cstdint>

namespace re4ab0 {
template <class E> uint32_t Do(E& e, uint32_t th, uint32_t unit, uint32_t a1, uint32_t qs, uint32_t out)
{
    (void)a1;
    auto q = [&](uint32_t g, uint32_t b) { return e.h(0xA568, 0, 0, 0, 0, 0, qs, g, b); };
    auto f = [&](uint32_t code) { return e.h(0xE660, unit, 0, 0, e.rd32(th + 4), 0, code); };
    e.wr8(out, 0);
    if (!q(0xF, 0)) return 1;
    const uint32_t fl = q(0x12, 2);
    if (fl || (e.rd8(th + 9) && e.rd8(th + 0xC) >= 2)) e.wr8(out, 1);
    if (f(0x20326671)) {
        const uint32_t c = e.h(0xE2960, e.rd32(th + 4), 0, 0, 0, 0);
        e.wr8(out, (c & 0xFF) ? 6 : 0xC);
        return (c & ~0xFFu) | 1;
    }
    uint32_t edi = 0, l18 = 0, l10 = 0, l14 = 0, d = 0;
    if (f(0x20316671)) { edi = 1; l18 = 1; }
    if (f(0x20796571)) { edi++; l10 = 1; }
    if (f(0x20726871)) { edi++; l14 = 1; }
    if (f(0x20726271)) { edi++; d = 1; }
    if (edi == 4) {
        const uint32_t v = q(0x12, 5);
        e.wr8(out, v ? 5 : 7);
        return ((0u - v) & ~0xFFu) | 1;
    }
    if (edi == 0) {
        if (fl) { e.wr8(out, 1); return (fl & ~0xFFu) | 1; }
        e.wr8(out, e.rd8(th + 0xC) > 1 ? 1 : 0);
        return (out & ~0xFFu) | 1;
    }
    if (!l10) { e.wr8(out, 1); return 1; }
    if (!d) { e.wr8(out, 2); return (out & ~0xFFu) | 1; }
    if (!l14) { e.wr8(out, 4); return 1; }
    e.wr8(out, l18 ? 7 : 3);
    return (out & ~0xFFu) | 1;
}
}
