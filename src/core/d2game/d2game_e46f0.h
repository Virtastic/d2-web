// 1.13c D2Game+0xE46F0, transcribed from the 1.13c disassembly: the Act 1 quest (group 0x13) update when the player picks up the quest item.
// Convention: ECX = game, EDX = player unit, ret 4 (one unused stack word); always returns 0. Quest flags: 0xA4C0 (player quest data) + 0x10 + 4 * difficulty
// (game byte +0x6D); read with 0xA568 (qs, group, bit), set with 0xA57A (same arguments). The quest node is the type 0x11 entry of the list at game +0x10F4.
#pragma once
#include <cstdint>

namespace re46f0 {
template <class E> uint32_t Do(E& e, uint32_t game, uint32_t unit, uint32_t a1)
{
    (void)a1;
    const uint32_t pq = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t qf = e.rd32(pq + e.rd8(game + 0x6D) * 4 + 0x10);
    if (e.h(0xA568, 0, 0, 0, 0, 0, qf, 0x13, 0) == 1) return 0;
    if (e.h(0xA568, 0, 0, 0, 0, 0, qf, 0x13, 6) == 1) return 0;
    const uint32_t list = e.rd32(game + 0x10F4);
    if (!list) return 0;
    uint32_t node = e.rd32(list);
    if (!node) return 0;
    while (e.rd32(node) != 0x11) { node = e.rd32(node + 0xF4); if (!node) return 0; }
    if (e.h(0xE660, unit, 0, 0, game, 0, 0x20333367)) { e.h(0xA57A, 0, 0, 0, 0, 0, qf, 0x13, 5); return 0; }
    const uint32_t d = e.rd32(node + 0x18);
    if (e.rd8(d)) {
        e.h(0xA57A, 0, 0, 0, 0, 0, qf, 0x13, 4);
        if (!e.h(0xA568, 0, 0, 0, 0, 0, qf, 0x13, 3)) e.h(0xA57A, 0, 0, 0, 0, 0, qf, 0x13, 2);
    } else if (e.rd8(node + 0xC) == 2) {
        e.h(0xA57A, 0, 0, 0, 0, 0, qf, 0x13, 3);
    }
    return 0;
}
}
