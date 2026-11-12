// 1.13c D2Game+0xE8560: object init callback of the third act's sage, his end spot (table entry). ecx = the init
// block ([0] game, [8] the room, [0x14] x, [0x18] y). Without the quest record 0x0E (d2game_objinit.h): 0. Its
// data's byte +0x02 becomes 1 and words +0x04, +0x08 take x, y. The control's quest flags (+0x0C; the original
// stops without its word +0x08) are asked for flag 0x10 bit 0x0D (0xA568: flags, 0x10, 0x0D); clear: 0. A set
// data byte +0x01 replaces the low byte of that answer as the result. With byte +0x00 set and its monster (id at
// +0x0C) still in the game, byte +0x03 becomes 1 and that monster is the result. Otherwise the monster 0xFD is
// spawned (0xEF650: ecx = 0xFD, edx = 1; stack: game, room, x, y, -1, 0); none: 0. The data keeps its id (+0x0C)
// and 1 in byte +0x01, the monster's flags (+0xC4) gain 0x3000000 and, with a map AI list at +0x18 whose word
// +0x04 is set and byte +0x10 clear, its map AI slot is pointed at it (0xA562: ecx = the list's address, edx =
// the slot), byte +0x10 becoming 1 and that answer the result; else the monster.
#pragma once
#include "d2game_objinit.h"

namespace ge8560 {
// the game's unit of an id in the monster buckets (+0x1320, 128 heads by the id's low 7 bits; chain at +0xE4)
template <class E> uint32_t Unit(E& e, uint32_t game, uint32_t id)
{
    for (uint32_t u = e.rd32(game + 0x1320 + (id & 0x7F) * 4); u; u = e.rd32(u + 0xE4))
        if (e.rd32(u + 0xC) == id) return u;
    return 0;
}
// the map AI slot of a monster: unit type 1 with data (+0x14): the data's word +0x28 plus 0x38; else 0x38
template <class E> uint32_t MapAi(E& e, uint32_t u)
{
    if (u && e.rd32(u) == 1 && e.rd32(u + 0x14)) return e.rd32(e.rd32(u + 0x14) + 0x28) + 0x38;
    return 0x38;
}

template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 0xE);
    if (!q) return 0;
    const uint32_t d = e.rd32(q + 0x18);
    e.wr8(d + 2, 1);
    e.wr32(d + 4, e.rd32(op + 0x14));
    e.wr32(d + 8, e.rd32(op + 0x18));
    const uint32_t f = e.h(0xA568, 0, 0, 0, 0, 0, e.rd32(e.rd32(e.rd32(op) + 0x10F4) + 0xC), 0x10, 0xD);
    if (!f) return 0;
    if (e.rd8(d + 1)) return (f & 0xFFFFFF00u) | e.rd8(d + 1);
    if (e.rd8(d)) {
        const uint32_t u = Unit(e, e.rd32(op), e.rd32(d + 0xC));
        if (u) { e.wr8(d + 3, 1); return u; }
    }
    const uint32_t u = e.h(0xEF650, 0, 0xFD, 1, 0, 0, e.rd32(op), e.rd32(op + 8), e.rd32(op + 0x14), e.rd32(op + 0x18), 0xFFFFFFFFu, 0);
    if (!u) return 0;
    e.wr32(d + 0xC, e.rd32(u + 0xC));
    e.wr8(d + 1, 1);
    e.wr32(u + 0xC4, e.rd32(u + 0xC4) | 0x3000000u);
    const uint32_t list = e.rd32(d + 0x18);
    if (!list || !e.rd32(list + 4) || e.rd8(d + 0x10)) return u;
    const uint32_t r = e.h(0xA562, 0, d + 0x18, MapAi(e, u));
    e.wr8(d + 0x10, 1);
    return r;
}
}
