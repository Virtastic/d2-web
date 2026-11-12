// 1.13c D2Game+0xE8EB0: object init callback "altar of the blade" (table entry). ecx = the init block ([0]
// game, [4] the object, [8] the room, [0x14] x, [0x18] y). Without the quest record 0x11 (d2game_objinit.h), or
// with its flag byte (+0x09) clear, the object goes to mode 2 (0xA4A2: object, mode) and that answer is the
// result. Otherwise the quest data's byte +0x18 becomes 1 and its words +0x08 and +0x0C take x and y; with the
// data's byte +0x03 set and +0x02 clear the boss is spawned (0xE8780: ebx = room, esi = data; stack: game) and
// its answer is the result, else the result is the data.
#pragma once
#include "d2game_objinit.h"

namespace ge8eb0 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 0x11);
    if (!q || !e.rd8(q + 9)) return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 2);
    const uint32_t d = e.rd32(q + 0x18);
    e.wr8(d + 0x18, 1);
    e.wr32(d + 8, e.rd32(op + 0x14));
    e.wr32(d + 0xC, e.rd32(op + 0x18));
    if (!e.rd8(d + 3) || e.rd8(d + 2)) return d;
    return e.h(0xE8780, 0, 0, 0, e.rd32(op + 8), d, e.rd32(op));
}
}
