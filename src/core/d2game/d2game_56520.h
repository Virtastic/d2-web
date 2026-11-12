// 1.13c D2Game+0x56520: object init callback of a portal of the last act's final quest (table entry).
// ecx = the init block ([0] game, [4] the object). The quest record 0x24 (d2game_objinit.h) is taken as present:
// the object goes to the mode kept in its data's word +0x94 (0xA4A2: object, mode), whose answer is the result,
// and that word becomes 2.
#pragma once
#include "d2game_objinit.h"

namespace g56520 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t d = e.rd32(gobjinit::Find(e, op, 0x24) + 0x18);
    const uint32_t r = e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), e.rd32(d + 0x94));
    e.wr32(d + 0x94, 2);
    return r;
}
}
