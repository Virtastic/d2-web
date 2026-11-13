// 1.13c D2Game+0xE45B0, transcribed from the 1.13c disassembly: the Act 1 quest (group 0x13) state step for a quest node.
// Convention: ECX = node (this), EDX = player unit, stack a1 (unused), qs (quest flag word), out (byte pointer); ret 0xC. Quest bits through 0xA568 (qs, group, bit);
// 0xE660 (EAX = unit, EBX = node +4, stack code) finds an item. Writes the state byte; AL = 1, the upper 24 bits of EAX are the last call's result (or the out
// pointer, the node +4 word, or a negated 0xA568 result) as the disassembly leaves them.
#pragma once
#include <cstdint>

namespace re45b0 {
template <class E> uint32_t Do(E& e, uint32_t th, uint32_t unit, uint32_t a1, uint32_t qs, uint32_t out)
{
    (void)a1;
    auto q = [&](uint32_t g, uint32_t b) { return e.h(0xA568, 0, 0, 0, 0, 0, qs, g, b); };
    uint32_t v = q(0xF, 0);
    if (!v) { e.wr8(out, 0); return (out & ~0xFFu) | 1; }
    v = q(0x13, 0);
    if (v) {
        v = q(0x13, 0xD);
        e.wr8(out, 0xB + (v ? 2 : 0));
        return ((0u - v) & ~0xFFu) | 1;
    }
    const uint32_t d = e.rd32(th + 0x18);
    e.wr8(out, 0);
    v = e.h(0xE660, unit, 0, 0, e.rd32(th + 4), 0, 0x20333367);
    if (v) { e.wr8(out, 4); return (v & ~0xFFu) | 1; }
    if (e.rd8(d + 0x20)) {
        const uint32_t rec = e.rd32(th + 4);
        e.wr8(out, 7 + (e.rd8(rec + 0x6A) == 3 ? 1 : 0));
        return (rec & ~0xFFu) | 1;
    }
    v = q(0x13, 6);
    if (v) {
        v = q(0x13, 8);
        if (!v) e.wr8(out, 6);
        v = q(0x13, 7);
        if (!v) e.wr8(out, 5);
        return (v & ~0xFFu) | 1;
    }
    v = q(0x13, 4);
    if (v) { e.wr8(out, 2 + (e.rd8(d) == 1 ? 1 : 0)); return (v & ~0xFFu) | 1; }
    v = q(0x13, 3);
    if (v) { e.wr8(out, 2); return (v & ~0xFFu) | 1; }
    v = q(0x13, 2);
    if (v) e.wr8(out, 1);
    return (v & ~0xFFu) | 1;
}
}
