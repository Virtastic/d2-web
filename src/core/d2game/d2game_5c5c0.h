// 1.13c D2Game+0x5C5C0: object init callback of the last act's fifth quest, the spawn spot (table entry). ecx =
// the init block ([0] game, [4] the object, [8] the room, [0x14] x, [0x18] y). Without the quest record 0x21
// (d2game_objinit.h): 0. Its data's word +0x110 becomes the object's id (-1 without an object). A set word +0x88
// or byte +0x93 is the result. Otherwise the quest monster is spawned (0x361E0: game, x, y, room, 1, 0x202); none:
// 0. The data keeps its id (+0x9C) and byte +0x93 becomes 1. With a map AI list at +0x108 whose word +0x04 is set
// and byte +0x10D clear, the monster's map AI slot is pointed at it (0xA562: ecx = the list's address, edx = the
// slot), byte +0x10D becomes 1 and that answer is the result; else the monster.
#pragma once
#include "d2game_objinit.h"

namespace g5c5c0 {
// the map AI slot of a monster: unit type 1 with data (+0x14): the data's word +0x28 plus 0x38; else 0x38
template <class E> uint32_t MapAi(E& e, uint32_t u)
{
    if (u && e.rd32(u) == 1 && e.rd32(u + 0x14)) return e.rd32(e.rd32(u + 0x14) + 0x28) + 0x38;
    return 0x38;
}

template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 0x21);
    if (!q) return 0;
    const uint32_t d = e.rd32(q + 0x18);
    e.wr32(d + 0x110, gobjinit::ObjId(e, e.rd32(op + 4)));
    if (e.rd32(d + 0x88)) return e.rd32(d + 0x88);
    if (e.rd8(d + 0x93)) return e.rd8(d + 0x93);
    const uint32_t u = e.h(0x361E0, 0, 0, 0, 0, 0, e.rd32(op), e.rd32(op + 0x14), e.rd32(op + 0x18), e.rd32(op + 8), 1, 0x202);
    if (!u) return 0;
    const uint32_t list = e.rd32(d + 0x108);
    e.wr32(d + 0x9C, e.rd32(u + 0xC));
    e.wr8(d + 0x93, 1);
    if (!list || !e.rd32(list + 4) || e.rd8(d + 0x10D)) return u;
    const uint32_t r = e.h(0xA562, 0, d + 0x108, MapAi(e, u));
    e.wr8(d + 0x10D, 1);
    return r;
}
}
