// 1.13c D2Game+0xE98E0: object init callback "bridge" (table entry). ecx = the init block ([0] game, [4] the
// object). Without the quest record 0x14 (d2game_objinit.h): 0. With it the quest data's byte +0x02 becomes 1
// and its word +0x08 the object's id (-1 without an object); the object goes to the data's mode (+0x10)
// (0xA4A2: object, mode). A mode other than 2 also queues an event (0x8EA80: ecx = game, edx = object; stack: 7,
// the game's frame (+0xA8) plus 0x14, 0, 0, 0). The last callee's answer is the result.
#pragma once
#include "d2game_objinit.h"

namespace ge98e0 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 0x14);
    if (!q) return 0;
    const uint32_t d = e.rd32(q + 0x18);
    e.wr8(d + 2, 1);
    e.wr32(d + 8, gobjinit::ObjId(e, e.rd32(op + 4)));
    const uint32_t r = e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), e.rd32(d + 0x10));
    if (e.rd32(d + 0x10) == 2) return r;
    const uint32_t game = e.rd32(op);
    return e.h(0x8EA80, 0, game, e.rd32(op + 4), 0, 0, 7, e.rd32(game + 0xA8) + 0x14, 0, 0, 0);
}
}
