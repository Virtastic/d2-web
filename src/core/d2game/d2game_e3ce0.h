// 1.13c D2Game+0xE3CE0, transcribed from the 1.13c disassembly: the Act 1 quest state step "state of the quest chain" for a quest node.
// Convention: ECX = node (this), EDX = player unit, stack a1 (unused), qs (quest flag word), out (byte pointer); ret 0xC. The quest bits are read through
// 0xA568 (D2Common 10174: qs, group, bit); 0xE660 (EAX = unit, EBX = node +4, stack code) finds an item. Writes the state byte and returns 1 in AL; the upper 24 bits of EAX are
// whatever the last call left (the out pointer, the last 0xA568 result, or its negation).
#pragma once
#include <cstdint>

namespace re3ce0 {
template <class E> uint32_t Do(E& e, uint32_t th, uint32_t unit, uint32_t a1, uint32_t qs, uint32_t out)
{
    (void)a1;
    auto q = [&](uint32_t g, uint32_t b) { return e.h(0xA568, 0, 0, 0, 0, 0, qs, g, b); };
    auto f = [&](uint32_t code) { return e.h(0xE660, unit, 0, 0, e.rd32(th + 4), 0, code); };
    uint32_t v = q(0xF, 0);
    if (!v) { e.wr8(out, 0); return (out & ~0xFFu) | 1; }
    v = q(0x14, 1);
    if (v) { e.wr8(out, 5); return (v & ~0xFFu) | 1; }
    v = q(0x14, 0);
    if (v) {
        v = q(0x14, 0xD);
        e.wr8(out, 0xB + (v ? 2 : 0));
        return ((0u - v) & ~0xFFu) | 1;
    }
    e.wr8(out, 0);
    const uint32_t r1 = f(0x20343367);
    const uint32_t r2 = f(0x2034336A);
    if (r1) {
        v = q(0x14, 4);
        e.wr8(out, 3 + (v ? 1 : 0));
        return (v & ~0xFFu) | 1;
    }
    if (r2) {
        v = q(0x14, 2);
        e.wr8(out, 1 + (v ? 1 : 0));
        return (v & ~0xFFu) | 1;
    }
    if (e.rd8(th + 9)) {
        uint32_t al = e.rd8(th + 0xB);
        if (al >= 6) al = 6 + (e.rd8(e.rd32(th + 4) + 0x6A) != 3 ? 1 : 0);
        e.wr8(out, al);
    }
    return 1;
}
}
