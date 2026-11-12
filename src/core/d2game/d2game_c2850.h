// 1.13c D2Game+0xC2850: object init callback "tomb seal" (table entry). ecx = the init block ([0] game, [4] the
// object). Without the quest record 0x0D (d2game_objinit.h) the object goes to mode 2 (0xA4A2: object, mode).
// With it the quest data's byte at +0x06 becomes 1 and its word at +0x28 the object's id (-1 without an
// object); the object then goes to mode 2 when the record's flag byte (+0x09) is clear or either data byte
// +0x0B or +0x0D is 1, else to mode 0. The callee's answer is the result.
#pragma once
#include "d2game_objinit.h"

namespace gc2850 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 0xD);
    if (!q) return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 2);
    const uint32_t d = e.rd32(q + 0x18);
    e.wr8(d + 6, 1);
    e.wr32(d + 0x28, gobjinit::ObjId(e, e.rd32(op + 4)));
    const bool two = !e.rd8(q + 9) || e.rd8(d + 0xB) == 1 || e.rd8(d + 0xD) == 1;
    return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), two ? 2 : 0);
}
}
