// 1.13c D2Game+0x60DA0: object init callback "tower tome" (table entry). ecx = the init block ([0] game, [4] the
// object). The quest record 5 (d2game_objinit.h) is required (the original stops without it). With its flag byte
// (+0x09) clear the object goes to mode 3 (0xA4A2: object, mode) and that answer is the result; else the record.
#pragma once
#include "d2game_objinit.h"

namespace g60da0 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 5);
    if (e.rd8(q + 9)) return q;
    return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 3);
}
}
