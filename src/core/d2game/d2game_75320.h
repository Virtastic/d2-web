// 1.13c D2Game+0x75320: monster mode callback that drops the unit's pending events (table entry). ecx = game, edx =
// the unit. The unit's event list (+0xDC, next at node +0x24) is walked; every node whose first byte is 0 is freed
// (0x8E6F0: eax = the node, edi = game), the next pointer being read before the call. Result 1.
#pragma once
#include <cstdint>

namespace g75320 {
template <class E> uint32_t DropEvents(E& e, uint32_t game, uint32_t unit)
{
    if (!unit) return 1;
    uint32_t node = e.rd32(unit + 0xDC);
    while (node) {
        const uint32_t next = e.rd32(node + 0x24);
        if (e.rd8(node) == 0) {
            e.h(0x8E6F0, node);
            e.j(0x8E6F0, game);
        }
        node = next;
    }
    return 1;
}
}
