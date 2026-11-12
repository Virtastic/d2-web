// 1.13c D2Game+0x58E50: object init callback of an ancient's statue (table entry). ecx = the init block
// ([0] game, [4] the object). The quest record 0x23 (d2game_objinit.h) is taken as present: its data's word +0x1C
// becomes the object's id (-1 without an object), and the shared statue step finishes (0x574F0: eax = block,
// ecx = game, edx = data), whose answer is the result.
#pragma once
#include "d2game_objinit.h"

namespace g58e50 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t d = e.rd32(gobjinit::Find(e, op, 0x23) + 0x18);
    e.wr32(d + 0x1c, gobjinit::ObjId(e, e.rd32(op + 4)));
    return e.h(0x574F0, op, e.rd32(op), d);
}
}
