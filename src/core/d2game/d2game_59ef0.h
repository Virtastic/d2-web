// 1.13c D2Game+0x59EF0: object init callback of the last act's fifth quest (table entry). ecx = the init block
// ([0] game, [4] the object). No quest control (game +0x10F4): 0; no record 0x21 in its list: the control. With
// the record its data's word +0xFC becomes the object's id (-1 without an object). The result is that id with its
// low byte replaced: 0 with the record's flag byte (+0x09) clear, 2 with its state byte (+0x0B) at 2 or more.
// Otherwise the state becomes 2, byte +0x14 is cleared and the players are walked (0xDFCC0: eax = 0, ebx = the
// handler, edi = the record's game (+0x04); stack: 0), whose answer is the result.
#pragma once
#include "d2game_objinit.h"

namespace g59ef0 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t ctl = e.rd32(e.rd32(op) + 0x10F4);
    if (!ctl) return 0;
    const uint32_t q = gobjinit::Find(e, op, 0x21);
    if (!q) return ctl;
    const uint32_t id = gobjinit::ObjId(e, e.rd32(op + 4));
    e.wr32(e.rd32(q + 0x18) + 0xFC, id);
    if (!e.rd8(q + 9)) return id & 0xFFFFFF00u;
    if (e.rd8(q + 0xB) >= 2) return (id & 0xFFFFFF00u) | 2;
    e.wr8(q + 0xB, 2);
    e.wr8(q + 0x14, 0);
    const uint32_t r = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0x591D0, 0, 0);
    e.j(0xDFCC0, e.rd32(q + 4));
    return r;
}
}
