// 1.13c D2Game+0x5DD00 ACT5Q3_ReleaseTrackedUnits: ESI = unit (a player: type 1 with pdata at +0x14), stack quest (ret 4); void.
// The pdata+0x30 list (first-node word; a node's +0 nonzero is a hit, next at +8): a hit calls 0x48410(game = quest+4, unit, 0).
// Then Ordinal 10159(unit+0x2C); every node of the unit+0xDC chain whose first byte is 2 (next at +0x24) goes to 0x8E6F0 (EAX = node,
// EDI = game); Ordinal 10887(unit, 6, 0, 0), 11103(unit, 0); 0x75E10(game, &block, 1) with block {0xC, unit, 0, 0, 0, 0x6400, 0, 0};
// Ordinal 10158(unit); unit+0xC4 |= 1. A unit that is not type 1, or has no pdata, makes retail read address 0 (callers never pass one).
#pragma once
#include <cstdint>

namespace d2g5dd00 {
template <class E> uint32_t FirstSet(E& e, uint32_t unit)
{
    uint32_t n = e.rd32(e.rd32(e.rd32(unit + 0x14) + 0x30));
    while (n && e.rd32(n) == 0) n = e.rd32(n + 8);
    return n;
}
template <class E> void Block(E& e, uint32_t game, uint32_t unit)
{
    uint32_t b[8] = { 0xC, unit, 0, 0, 0, 0x6400, 0, 0 };
    e.h(0x75E10, 0, 0, 0, 0, 0, game, (uint32_t)(uintptr_t)b, 1);
    e.blk(0x75E10, b, 8);
}
template <class E> void Run(E& e, uint32_t unit, uint32_t quest)
{
    if (FirstSet(e, unit)) e.h(0x48410, 0, 0, 0, 0, 0, e.rd32(quest + 4), unit, 0);
    e.h(0xAB50, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    const uint32_t game = e.rd32(quest + 4);
    for (uint32_t n = e.rd32(unit + 0xDC); n;) {
        const uint32_t nx = e.rd32(n + 0x24);
        if (e.rd8(n) == 2) { e.h(0x8E6F0, n); e.edi(0x8E6F0, game); }
        n = nx;
    }
    e.h(0xA472, 0, 0, 0, 0, 0, unit, 6, 0, 0);
    e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
    Block(e, game, unit);
    e.h(0xA4DE, 0, 0, 0, 0, 0, unit);
    e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 1);
}
}
