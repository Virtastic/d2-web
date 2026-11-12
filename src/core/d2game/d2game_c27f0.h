// 1.13c D2Game+0xC27F0: object init callback "tomb door" (table entry). ecx = the init block ([0] game, [4] the
// object). Without the quest record 0x0D (d2game_objinit.h): 0. With it the quest data's byte at +0x05 becomes
// 1 and its word at +0x2C the object's id (-1 without an object); the object goes to mode 2 when the record's
// flag byte (+0x09) is clear, else to the data's mode (+0x18) (0xA4A2: object, mode). The callee's answer is the
// result.
#pragma once
#include "d2game_objinit.h"

namespace gc27f0 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 0xD);
    if (!q) return 0;
    const uint32_t d = e.rd32(q + 0x18);
    e.wr8(d + 5, 1);
    e.wr32(d + 0x2C, gobjinit::ObjId(e, e.rd32(op + 4)));
    if (!e.rd8(q + 9)) return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 2);
    return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), e.rd32(d + 0x18));
}
}
