// 1.13c D2Game+0xE86A0: object init callback of the third act's sage, his start spot (table entry). ecx = the
// init block ([0] game, [8] the room, [0x14] x, [0x18] y). No quest control (game +0x10F4): 0; no record 0x0E in
// its list: the control. The control's quest flags (+0x0C; the original stops without its word +0x08) are asked
// for flag 0x10 bit 0x0D (0xA568: flags, 0x10, 0x0D); a set bit is the result. With the data's byte +0x00 set and
// its monster (id at +0x0C) still in the game, that monster is the result. Otherwise the monster 0xFD is spawned
// (0xEF650: ecx = 0xFD, edx = 1; stack: game, room, x, y, -1, 0); none: 0; else the data keeps its id (+0x0C) and
// 1 in byte +0x00, and the id is the result.
#pragma once
#include "d2game_objinit.h"

namespace ge86a0 {
// the game's unit of an id in the monster buckets (+0x1320, 128 heads by the id's low 7 bits; chain at +0xE4)
template <class E> uint32_t Unit(E& e, uint32_t game, uint32_t id)
{
    for (uint32_t u = e.rd32(game + 0x1320 + (id & 0x7F) * 4); u; u = e.rd32(u + 0xE4))
        if (e.rd32(u + 0xC) == id) return u;
    return 0;
}

template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t ctl = e.rd32(e.rd32(op) + 0x10F4);
    if (!ctl) return 0;
    const uint32_t q = gobjinit::Find(e, op, 0xE);
    if (!q) return ctl;
    const uint32_t f = e.h(0xA568, 0, 0, 0, 0, 0, e.rd32(ctl + 0xC), 0x10, 0xD);
    if (f) return f;
    const uint32_t d = e.rd32(q + 0x18);
    if (e.rd8(d)) {
        const uint32_t u = Unit(e, e.rd32(op), e.rd32(d + 0xC));
        if (u) return u;
    }
    const uint32_t u = e.h(0xEF650, 0, 0xFD, 1, 0, 0, e.rd32(op), e.rd32(op + 8), e.rd32(op + 0x14), e.rd32(op + 0x18), 0xFFFFFFFFu, 0);
    if (!u) return 0;
    e.wr32(d + 0xC, e.rd32(u + 0xC));
    e.wr8(d, 1);
    return e.rd32(u + 0xC);
}
}
