// 1.13c D2Game+0xB8870: object init callback "last portal" (table entry). ecx = the init block ([0] game, [4] the
// object). The game's quest control (+0x10F4) heads a list of quest records (first at +0x00, next at +0xF4); the
// record with id 0x17 (+0x00) is looked up. No control, an empty list or no such record: 0. Otherwise the record's
// data (+0x18) keeps a portal state at +0x40: already 2, the object goes to mode 2; else the state becomes 2 and
// the object goes to mode 1 (0xA4A2: object, mode). That call's result is the result.
#pragma once
#include <cstdint>

namespace gb8870 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    uint32_t q = e.rd32(e.rd32(op) + 0x10F4);
    if (!q) return 0;
    for (q = e.rd32(q); q; q = e.rd32(q + 0xF4))
        if (e.rd32(q) == 0x17) {
            const uint32_t d = e.rd32(q + 0x18);
            if (e.rd32(d + 0x40) == 2) return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 2);
            e.wr32(d + 0x40, 2);
            return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 1);
        }
    return 0;
}
}
