// 1.13c D2Game+0x3E670: monster skill callback (table entry). ecx = game, edx = a unit, one stack word (ret 4).
// A nonzero stack word gives 0. The unit's id (0xB24C) is looked up in the game's 128-bucket unit list at
// +0x1520 (chained at +0xE4, id at +0x0C; a null game is the fatal assert, line 0x2D3); not found: 0. The found
// unit's room (0xA478), when there is one, is flagged (0xA51A: room, 1). When the unit's word at +0x10 is zero
// it is flagged too (0xA4A2: unit, 1) and, with a record for its class (0xA520: +4), a timed event is queued
// (0x8EA80: ecx = game, edx = unit; 1, the record's word +0xDC >> 8 plus the game's frame +0xA8, 0, 0, 0).
// The result is then 1.
#pragma once
#include <cstdint>
#include "d2game_fatal.h"

namespace g3e670 {
template <class E> uint32_t Do(E& e, uint32_t game, uint32_t unit, uint32_t s0)
{
    if (s0) return 0;
    uint32_t id = e.h(0xB24C, 0, 0, 0, 0, 0, unit);
    if (!game) d2gfatal::Fatal(e, 0x2D3, 0);
    uint32_t node = e.rd32(game + 0x1520 + 4 * (id & 0x7F));
    while (node && e.rd32(node + 0xC) != id) node = e.rd32(node + 0xE4);
    if (!node) return 0;
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, node);
    if (room) e.h(0xA51A, 0, 0, 0, 0, 0, room, 1);
    if (e.rd32(node + 0x10) == 0) {
        e.h(0xA4A2, 0, 0, 0, 0, 0, node, 1);
        const uint32_t q = e.h(0xA520, 0, 0, 0, 0, 0, e.rd32(node + 4));
        if (q) {
            e.h(0x8EA80, 0, game, node, 0, 0, 1, (e.rd32(q + 0xDC) >> 8) + e.rd32(game + 0xA8), 0, 0);
            const uint32_t w[1] = { 0 };
            e.x(0x8EA80, w, 1);
        }
    }
    return 1;
}
}
