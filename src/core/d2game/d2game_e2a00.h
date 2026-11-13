// 1.13c D2Game+0xE2A00: object init callback "stairs" (table entry). ecx = the init block ([0] game, [4] the
// object). The quest record 0x13 (d2game_objinit.h) is taken as present. With its data's byte +0x0C set the object
// goes to mode 2 (0xA4A2: object, mode) and that answer is the result; else EAX is the quest record with its low
// byte replaced by that (zero) data byte.
#pragma once
#include "d2game_objinit.h"

namespace ge2a00 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t node = gobjinit::Find(e, op, 0x13);
    const uint32_t d = e.rd32(node + 0x18);
    if (!e.rd8(d + 0xC)) return node & 0xFFFFFF00u;   // mov al, [d+0xC] over the live EAX = the quest record
    return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 2);
}
}
