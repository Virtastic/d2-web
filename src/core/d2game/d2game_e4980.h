// 1.13c D2Game+0xE4980: object init callback "sewer stairs" (table entry). ecx = the init block ([0] game, [4]
// the object). Without the quest record 0x10 (d2game_objinit.h): 0. With it the quest data's first byte becomes
// 1 and its word at +0x04 the object's id (-1 without an object); the object goes to mode 2 when the record's
// flag byte (+0x09) is clear, else to the data's mode (+0x08) (0xA4A2: object, mode). The callee's answer is the
// result.
#pragma once
#include "d2game_objinit.h"

namespace ge4980 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 0x10);
    if (!q) return 0;
    const uint32_t d = e.rd32(q + 0x18);
    e.wr8(d, 1);
    e.wr32(d + 4, gobjinit::ObjId(e, e.rd32(op + 4)));
    if (!e.rd8(q + 9)) return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 2);
    return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), e.rd32(d + 8));
}
}
