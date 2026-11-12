// 1.13c D2Game+0x67D20: object init callback "cairn stone" (table entry). ecx = the init block ([0] game, [4]
// the object). Without the quest record 4 (d2game_objinit.h): an object already in mode 2 (+0x10) is the result,
// else it goes to mode 2 (0xA4A2: object, mode). With the record, c = the object's class (+0x04; -1 without an
// object) and d = the quest data. Flag byte (+0x09) set and d byte +0x4C clear: with d byte +0x4D or +0x50 set
// the object goes to mode 2; else a stone byte (d + c + 0x4B) other than 1 answers d, and a 1 is cleared with the
// object going to mode 0. Otherwise byte +0x4C is cleared; with byte +0x45 clear and class 0x11, word +0x40 takes
// the object's id and, byte +0x44 clear, it becomes 1 and a quest timer starts (0x33AF0: record, the handler);
// the object goes to mode 2. The last mode change's answer is the result.
#pragma once
#include "d2game_objinit.h"

namespace g67d20 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 4);
    const uint32_t obj = e.rd32(op + 4);
    if (!q) {
        if (obj && e.rd32(obj + 0x10) == 2) return obj;
        return e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 2);
    }
    const uint32_t c = obj ? e.rd32(obj + 4) : 0xFFFFFFFFu;
    const uint32_t d = e.rd32(q + 0x18);
    if (e.rd8(q + 9) && !e.rd8(d + 0x4C)) {
        if (e.rd8(d + 0x4D) || e.rd8(d + 0x50)) return e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 2);
        if (e.rd8(d + c + 0x4B) != 1) return d;
        e.wr8(d + c + 0x4B, 0);
        return e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 0);
    }
    const bool open = !e.rd8(d + 0x45);
    e.wr8(d + 0x4C, 0);
    if (open && c == 0x11) {
        e.wr32(d + 0x40, gobjinit::ObjId(e, obj));
        if (!e.rd8(d + 0x44)) {
            e.wr8(d + 0x44, 1);
            e.h(0x33AF0, 0, 0, 0, 0, 0, q, 0xB2000000u | 0x67A90);
        }
    }
    return e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 2);
}
}
