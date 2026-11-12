// 1.13c D2Common+0x472E0: item property callback "set indestructible" (table entry). ecx unread, edx = the second
// register word, nine stack words (ret 0x24): the item, the property record, ..., words 7 and 8 passed on. The
// item must be a unit of type 4, the record non-null and the stat table must hold more than 0x98 records (data
// tables +0xBD4). The item's stat list is fetched (0x47020: eax = item, ecx = the edx word; stack: words 7 and 8);
// none: 0. Stat 0x98 is set to 1 on it (0x3A330: list, 0x98, 1, 0) and the result is 1.
#pragma once
#include <cstdint>

namespace c472e0 {
template <class E> uint32_t Apply(E& e, uint32_t dt, uint32_t d, uint32_t item, uint32_t rec, uint32_t s6, uint32_t s7)
{
    if (!item || e.rd32(item) != 4 || !rec) return 0;
    if ((int32_t)e.rd32(dt + 0xBD4) <= 0x98) return 0;
    const uint32_t list = e.h(0x47020, item, d, 0, 0, 0, s6, s7);
    if (!list) return 0;
    e.h(0x3A330, 0, 0, 0, 0, 0, list, 0x98, 1, 0);
    return 1;
}
}
