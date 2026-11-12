// 1.13c D2Game+0x628D0: object init callback "invisible quest object" (table entry). ecx = the init block ([0]
// game, [4] the object). Without the quest record 4 (d2game_objinit.h) an object already in mode 2 (+0x10) is
// left alone (the result is the object), any other goes to mode 2 (0xA4A2: object, mode). With the record the
// lookup is repeated in the record's own game (+0x04): no control there: 0; no head or no record 4: the result
// is that control. With the second record, an object of type 2 (+0x00) and class 0x3D (+0x04) marks that
// record's data (byte +0x49 = 1, word +0x28 = the object's id); then the quest's state is pushed out (0x33F80:
// esi = second record, edi = object; stack: the record's game) and its answer is the result.
#pragma once
#include "d2game_objinit.h"

namespace g628d0 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 4);
    const uint32_t obj = e.rd32(op + 4);
    if (!q) {
        if (obj && e.rd32(obj + 0x10) == 2) return obj;
        return e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 2);
    }
    const uint32_t game = e.rd32(q + 4);
    const uint32_t ctl = e.rd32(game + 0x10F4);
    if (!ctl) return 0;
    uint32_t q2 = e.rd32(ctl);
    for (; q2; q2 = e.rd32(q2 + 0xF4))
        if (e.rd32(q2) == 4) break;
    if (!q2) return ctl;
    if (obj && e.rd32(obj) == 2 && e.rd32(obj + 4) == 0x3D) {
        const uint32_t d = e.rd32(q2 + 0x18);
        e.wr8(d + 0x49, 1);
        e.wr32(d + 0x28, e.rd32(obj + 0xC));
    }
    const uint32_t r = e.h(0x33F80, 0, 0, 0, 0, q2, game);
    e.j(0x33F80, obj);
    return r;
}
}
