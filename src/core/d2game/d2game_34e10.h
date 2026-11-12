// 1.13c D2Game+0x34E10: object init callback "malus stand" (table entry). ecx = the init block ([0] game, [4]
// the object). Without the quest record 3 (d2game_objinit.h) an object already in mode 2 (+0x10) is left alone
// (the result is the object), any other goes to mode 2 (0xA4A2: object, mode). With the record the quest data's
// byte at +0x01 becomes 1 and its word at +0x04 the object's id (-1 without an object); a clear record flag
// (+0x09) makes the data's mode word (+0x98) 2; the object goes to that mode. The callee's answer is the result.
#pragma once
#include "d2game_objinit.h"

namespace g34e10 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 3);
    const uint32_t obj = e.rd32(op + 4);
    if (!q) {
        if (obj && e.rd32(obj + 0x10) == 2) return obj;
        return e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 2);
    }
    const uint32_t d = e.rd32(q + 0x18);
    e.wr8(d + 1, 1);
    e.wr32(d + 4, gobjinit::ObjId(e, obj));
    if (!e.rd8(q + 9)) e.wr32(d + 0x98, 2);
    return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), e.rd32(d + 0x98));
}
}
