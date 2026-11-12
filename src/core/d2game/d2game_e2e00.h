// 1.13c D2Game+0xE2E00: object init callback "sewer lever" (table entry). ecx = the init block ([0] game, [4] the
// object). The game's quest list (+0x10F4, first node at its word 0, next at node +0xF4) is walked for quest id
// 16. No list or no such quest: 0. A quest whose byte +0x09 is set comes back as is. Otherwise the object is put
// in mode 2 (0xA4A2: object, 2) and that call's result is the result.
#pragma once
#include <cstdint>

namespace ge2e00 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t list = e.rd32(e.rd32(op) + 0x10F4);
    if (!list) return 0;
    for (uint32_t q = e.rd32(list); q; q = e.rd32(q + 0xF4)) {
        if (e.rd32(q) != 16) continue;
        if (e.rd8(q + 9)) return q;
        return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 2);
    }
    return 0;
}
}
