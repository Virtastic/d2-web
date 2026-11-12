// 1.13c D2Game+0x34DA0: object init callback "gibbet" (table entry). ecx = the init block ([0] game, [4] the
// object). Without the quest record 4 (d2game_objinit.h) an object already in mode 2 (+0x10) is left alone
// (result 0), any other goes to mode 2 (0xA4A2: object, mode). With the record the object goes to the mode kept
// in the quest data (+0x54), the data's byte at +0x48 becomes 1 and its word at +0x34 the object's id (-1
// without an object). The callee's answer is the result.
#pragma once
#include "d2game_objinit.h"

namespace g34da0 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 4);
    const uint32_t obj = e.rd32(op + 4);
    if (!q) {
        if (obj && e.rd32(obj + 0x10) == 2) return 0;
        return e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 2);
    }
    const uint32_t d = e.rd32(q + 0x18);
    const uint32_t r = e.h(0xA4A2, 0, 0, 0, 0, 0, obj, e.rd32(d + 0x54));
    e.wr8(d + 0x48, 1);
    e.wr32(d + 0x34, gobjinit::ObjId(e, e.rd32(op + 4)));
    return r;
}
}
