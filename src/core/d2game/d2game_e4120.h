// 1.13c D2Game+0xE4120: object init callback "Gidbinn altar" (table entry). ecx = the init block ([0] game, [4]
// the object, [0x14] x, [0x18] y). The game's quest control (+0x10F4) heads a list of quest records (first at
// +0x00, next at +0xF4); the record with id 0x11 (+0x00) is looked up. No control, an empty list or no such
// record: 0. Otherwise the record's data (+0x18) receives the object's id (+0x0C; -1 without an object) at +0x28
// and the position at +0x10 and +0x14, and the object takes the mode kept at data +0x2C (0xA4A2: object, mode);
// that call's result is the result.
#pragma once
#include <cstdint>

namespace ge4120 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    uint32_t q = e.rd32(e.rd32(op) + 0x10F4);
    if (!q) return 0;
    for (q = e.rd32(q); q; q = e.rd32(q + 0xF4))
        if (e.rd32(q) == 0x11) {
            const uint32_t obj = e.rd32(op + 4);
            const uint32_t d = e.rd32(q + 0x18);
            e.wr32(d + 0x28, obj ? e.rd32(obj + 0xC) : 0xFFFFFFFFu);
            e.wr32(d + 0x10, e.rd32(op + 0x14));
            e.wr32(d + 0x14, e.rd32(op + 0x18));
            return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), e.rd32(d + 0x2C));
        }
    return 0;
}
}
