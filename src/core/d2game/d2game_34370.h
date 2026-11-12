// 1.13c D2Game+0x34370: object init callback "tome stand" (table entry). ecx = the init block ([0] game, [4]
// the object). The quest record 0x0F is looked up (d2game_objinit.h); a missing record ends in the
// assert-and-exit stub, which is not modelled. With the record's flag byte (+0x09) clear the object goes to
// mode 2 (0xA4A2: object, mode), else to the mode in its quest data (+0x18 of the data). The callee's answer is
// the result.
#pragma once
#include "d2game_objinit.h"

namespace g34370 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 0xF);
    const uint32_t obj = e.rd32(op + 4);
    if (!e.rd8(q + 9)) return e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 2);
    return e.h(0xA4A2, 0, 0, 0, 0, 0, obj, e.rd32(e.rd32(q + 0x18) + 0x18));
}
}
