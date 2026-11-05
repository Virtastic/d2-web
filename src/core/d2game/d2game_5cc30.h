// Quest-node arm: ECX = game. Walks the game's quest list (the head pointer at game+0x10F4; node +0 id, +0xF4 next) to the first
// node whose id is 0x21; on that node's state (+0x18) it sets the byte +0xAD to 1 and the word +0x84 to 2, then runs the node step
// with EAX = node, ECX = state and both as its two stack words. No list, no head, or no match: nothing happens.
#pragma once
#include <cstdint>

namespace d2g5cc30 {
template <class E> void Run(E& e, uint32_t game)
{
    // 1.13c D2Game+0x5CC30
    const uint32_t list = e.rd32(game + 0x10F4);
    if (!list) return;
    uint32_t node = e.rd32(list);
    if (!node) return;
    while (e.rd32(node) != 0x21) {
        node = e.rd32(node + 0xF4);
        if (!node) return;
    }
    const uint32_t st = e.rd32(node + 0x18);
    e.wr8(st + 0xAD, 1);
    e.wr32(st + 0x84, 2);
    e.h(0x5C690, node, st, 0, 0, 0, node, st);
}
}
