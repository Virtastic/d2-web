// 1.13c D2Game+0xE2A00: object init callback "stairs" (table entry). ecx = the init block ([0] game, [4] the
// object). The quest record 0x13 (d2game_objinit.h) is taken as present. With its data's byte +0x0C set the object
// goes to mode 2 (0xA4A2: object, mode) and that answer is the result; else the data's address with its low byte
// cleared.
#pragma once
#include "d2game_objinit.h"

namespace ge2a00 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t d = e.rd32(gobjinit::Find(e, op, 0x13) + 0x18);
    if (!e.rd8(d + 0xC)) return d & 0xFFFFFF00u;
    return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 2);
}
}
