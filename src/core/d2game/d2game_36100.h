// 1.13c D2Game+0x36100: object init callback "tainted sun altar" (table entry). ecx = the init block ([0] game,
// [4] the object). The game's quest list (+0x10F4, first node at its word 0, next at node +0xF4) is walked for
// quest id 10. Found: the quest's own object init (0xC71A0: the block) runs and its result is the result. Not
// found: an object already in mode 2 (+0x10) comes back as is; any other (or none) is put in mode 2 (0xA4A2:
// object, 2) and that call's result is the result.
#pragma once
#include <cstdint>

namespace g36100 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t list = e.rd32(e.rd32(op) + 0x10F4);
    if (list)
        for (uint32_t q = e.rd32(list); q; q = e.rd32(q + 0xF4))
            if (e.rd32(q) == 10) return e.h(0xC71A0, 0, 0, 0, 0, 0, op);
    const uint32_t obj = e.rd32(op + 4);
    if (obj && e.rd32(obj + 0x10) == 2) return obj;
    return e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 2);
}
}
