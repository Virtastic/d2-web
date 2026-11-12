// 1.13c D2Game+0x574A0: object init callback "Ancients' altar" (table entry). ecx = the init block ([0] game, [4]
// the object). The game's quest control (+0x10F4) heads a list of quest records (first at +0x00, next at +0xF4);
// the record with id 0x23 (+0x00) is looked up. No control, an empty list or no such record: 0. Otherwise the
// record's data (+0x18) receives the object's id (+0x0C; -1 without an object) at +0x50, and the object takes
// the mode kept at data +0x4C (0xA4A2: object, mode); that call's result is the result.
#pragma once
#include <cstdint>

namespace g574a0 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    uint32_t q = e.rd32(e.rd32(op) + 0x10F4);
    if (!q) return 0;
    for (q = e.rd32(q); q; q = e.rd32(q + 0xF4))
        if (e.rd32(q) == 0x23) {
            const uint32_t obj = e.rd32(op + 4);
            e.wr32(e.rd32(q + 0x18) + 0x50, obj ? e.rd32(obj + 0xC) : 0xFFFFFFFFu);
            const uint32_t mode = e.rd32(e.rd32(q + 0x18) + 0x4C);
            return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), mode);
        }
    return 0;
}
}
