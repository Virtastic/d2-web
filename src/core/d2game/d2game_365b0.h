// 1.13c D2Game+0x365B0: object init callback (table entry). ecx = the init block, whose first word is the game.
// The game's quest list (+0x10F4, first node at its word 0, next at node +0xF4) is walked for quest id 11. No list:
// 0. No such quest: the list pointer comes back. Found: the quest's start-object init (0xC7C20: ecx and edi = the
// block, esi = the quest node) runs and its result is the result.
#pragma once
#include <cstdint>

namespace g365b0 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t list = e.rd32(e.rd32(op) + 0x10F4);
    if (!list) return 0;
    for (uint32_t q = e.rd32(list); q; q = e.rd32(q + 0xF4)) {
        if (e.rd32(q) != 11) continue;
        const uint32_t r = e.h(0xC7C20, 0, op, 0, 0, q);
        e.j(0xC7C20, op);
        return r;
    }
    return list;
}
}
