// 1.13c D2Game+0x36C20: object init callback (table entry). ecx = the init block, whose first word is the game.
// The game's quest list (+0x10F4, first node at its word 0, next at node +0xF4) is walked for quest id 11. Found:
// the quest's palace-object init (0xC9B70: eax = the block, ecx = the game, edx and one stack word = the quest
// node) runs and its result is the result. Otherwise the block pointer comes back.
#pragma once
#include <cstdint>

namespace g36c20 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t game = e.rd32(op);
    const uint32_t list = e.rd32(game + 0x10F4);
    if (!list) return op;
    for (uint32_t q = e.rd32(list); q; q = e.rd32(q + 0xF4))
        if (e.rd32(q) == 11) return e.h(0xC9B70, op, game, q, 0, 0, q);
    return op;
}
}
