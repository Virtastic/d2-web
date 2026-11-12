// 1.13c D2Game+0x614A0: object init callback "tree with the scroll" (table entry). ecx = the init block ([0]
// game, [4] the object). Without the quest record 4 (d2game_objinit.h) an object already in mode 2 (+0x10) is
// left alone (the result is the object), any other goes to mode 2 (0xA4A2: object, mode). With the record the
// quest data's byte at +0x47 becomes 1; a clear record flag (+0x09) or a set data byte at +0x50 makes the data's
// mode word (+0x58) 1; the object goes to that mode. The callee's answer is the result.
#pragma once
#include "d2game_objinit.h"

namespace g614a0 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 4);
    const uint32_t obj = e.rd32(op + 4);
    if (!q) {
        if (obj && e.rd32(obj + 0x10) == 2) return obj;
        return e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 2);
    }
    const uint32_t d = e.rd32(q + 0x18);
    e.wr8(d + 0x47, 1);
    if (!e.rd8(q + 9) || e.rd8(d + 0x50)) e.wr32(d + 0x58, 1);
    return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), e.rd32(d + 0x58));
}
}
