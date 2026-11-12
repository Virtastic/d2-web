// 1.13c D2Game+0x565B0: object init callback "Ancients' door" (table entry). ecx = the init block ([0] game, [4]
// the object). The game's quest control (+0x10F4) heads a list of quest records (first at +0x00, next at +0xF4);
// the record with id 0x23 (+0x00) is looked up. No control, an empty list or no such record: 0. Otherwise the
// record's data (+0x18) holds a "door open" byte (+0x4A): set, the object goes to mode 2, clear, to mode 0
// (0xA4A2: object, mode); that call's result is the result.
#pragma once
#include <cstdint>

namespace g565b0 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    uint32_t q = e.rd32(e.rd32(op) + 0x10F4);
    if (!q) return 0;
    for (q = e.rd32(q); q; q = e.rd32(q + 0xF4))
        if (e.rd32(q) == 0x23) {
            const uint32_t mode = e.rd8(e.rd32(q + 0x18) + 0x4A) ? 2 : 0;
            return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), mode);
        }
    return 0;
}
}
