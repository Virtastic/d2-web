// 1.13c D2Game+0x5CF70 ACT5Q3_TickPlaceObject: ECX = event record p (+0 game, +8, +0x14, +0x18), plain ret; result unused.
// Walks the game's quest list (game+0x10F4 points at the first-node word; type dword at +0, next at +0xF4) for type 0x21. Its state
// (node+0x18) must have +0x88 == 1 and byte +0x92 clear; then Ordinal 10285(2, word [sgptDataTables]+0xB58) feeds 0x47CF0 (EAX = that result,
// ECX = p+8, stack game, p+0x14, p+0x18, 1). A nonzero result's +0xC goes to state+0xA0 and state+0x92 becomes 1.
// dtCell is the address of the sgptDataTables word (the module's import slot points at it).
#pragma once
#include <cstdint>

namespace d2g5cf70 {
template <class E> void Run(E& e, uint32_t p, uint32_t dtCell)
{
    const uint32_t game = e.rd32(p);
    const uint32_t lst = e.rd32(game + 0x10F4);
    if (!lst) return;
    uint32_t node = e.rd32(lst);
    while (node && e.rd32(node) != 0x21) node = e.rd32(node + 0xF4);
    if (!node) return;
    const uint32_t st = e.rd32(node + 0x18);
    if (e.rd32(st + 0x88) != 1 || e.rd8(st + 0x92)) return;
    const uint32_t x = e.h(0xADF6, 0, 0, 0, 0, 0, 2, e.rd32(e.rd32(dtCell) + 0xB58) & 0xFFFF);
    const uint32_t res = e.h(0x47CF0, x, e.rd32(p + 8), 0, 0, 0, game, e.rd32(p + 0x14), e.rd32(p + 0x18), 1);
    if (!res) return;
    e.wr32(st + 0xA0, e.rd32(res + 0xC));
    e.wr8(st + 0x92, 1);
}
}
