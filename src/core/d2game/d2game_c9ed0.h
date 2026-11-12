// 1.13c D2Game+0xC9ED0: object init callback "orifice" (table entry). ecx = the init block ([0] game, [4] the
// object). Without the quest record 0x0D (d2game_objinit.h): 0. With it the quest data's byte at +0x04 becomes
// 1 and its word at +0x20 the object's id (-1 without an object). When the data's byte +0x0C is set and its
// bytes +0x03 and +0x0B are clear, a quest timer is started (0x33AF0: record, the timer handler), byte +0x03
// becomes 1 and the object goes to mode 2 (0xA4A2: object, mode). Otherwise a set byte +0x0B alone sends the
// object to mode 2. The result is the last callee's answer, or, with no mode change, the record's address with
// its low byte replaced by the data's byte +0x0B (zero there).
#pragma once
#include "d2game_objinit.h"

namespace gc9ed0 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 0xD);
    if (!q) return 0;
    const uint32_t d = e.rd32(q + 0x18);
    e.wr8(d + 4, 1);
    e.wr32(d + 0x20, gobjinit::ObjId(e, e.rd32(op + 4)));
    if (e.rd8(d + 0xC) && !e.rd8(d + 3) && !e.rd8(d + 0xB)) {
        e.h(0x33AF0, 0, 0, 0, 0, 0, q, 0xB2000000u | 0xC94E0);
        e.wr8(d + 3, 1);
        return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 2);
    }
    const uint32_t b = e.rd8(d + 0xB);
    if (!b) return q & 0xFFFFFF00u;
    return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 2);
}
}
