// 1.13c D2Game+0xE8290: object init callback "wanderer spot" (table entry). ecx = the init block ([0] game, [8]
// the room, [0x14] x, [0x18] y). Without the quest record 0x1C (d2game_objinit.h): 0. With the quest data's
// byte +0x01 clear, the data's first byte becomes 1 and the result is the record's address with its low byte
// cleared. Otherwise the data keeps x + 7 (+0x04) and y (+0x08), the room of that spot is found (0x1280: eax =
// room, ebx = x + 7, edi = y) and the monster 0x170 is spawned there (0xEF650: ecx = 0x170, edx = 1; stack:
// game, room, x, y, -1, 0); a spawn clears byte +0x01. The first byte becomes 1; the result is the spawn's
// answer.
#pragma once
#include "d2game_objinit.h"

namespace ge8290 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 0x1C);
    if (!q) return 0;
    const uint32_t d = e.rd32(q + 0x18);
    if (!e.rd8(d + 1)) { e.wr8(d, 1); return q & 0xFFFFFF00u; }
    const uint32_t x = e.rd32(op + 0x14) + 7, y = e.rd32(op + 0x18);
    e.wr32(d + 4, x);
    e.wr32(d + 8, y);
    const uint32_t room = e.h(0x1280, e.rd32(op + 8), 0, 0, x);
    e.j(0x1280, y);
    const uint32_t r = e.h(0xEF650, 0, 0x170, 1, 0, 0, e.rd32(op), room, e.rd32(d + 4), e.rd32(d + 8), 0xFFFFFFFFu, 0);
    if (r) e.wr8(d + 1, 0);
    e.wr8(d, 1);
    return r;
}
}
