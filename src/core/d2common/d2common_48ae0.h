// 1.13c D2Common+0x48AE0: item property callback "add a rolled stat, clearing it first on an item in mode 1"
// (table entry). ecx = the mode word, edx = the second register word, nine stack words (ret 0x24): the item, the
// property record, words 2 and 3 (3 = the stat), ..., words 6 and 7 passed on. No record: 0. An item unit (type 4)
// in mode 1 first clears the stat (0x46E50: esi = item; stack: the stat as a signed 16-bit word). A value is
// rolled between the record's bounds (d2common_roll.h) and the stat applied (0x475E0: eax = record; stack: edx
// word, item, words 2 and 3, 0, the roll, words 6 and 7); that call's result is the result.
#pragma once
#include <cstdint>
#include "d2common_roll.h"

namespace c48ae0 {
template <class E> uint32_t Apply(E& e, uint32_t c, uint32_t d, uint32_t item, uint32_t rec, uint32_t s2, uint32_t s3, uint32_t s6, uint32_t s7)
{
    if (!rec) return 0;
    if (item && e.rd32(item) == 4 && c == 1) e.h(0x46E50, 0, 0, 0, 0, item, (uint32_t)(int32_t)(int16_t)s3);
    const uint32_t roll = D2CommonPropRoll(e, item, rec);
    const uint32_t res = e.h(0x475E0, rec, 0, 0, 0, 0, d, item, s2, s3);
    const uint32_t w[4] = { 0, roll, s6, s7 };
    e.x(0x475E0, w, 4);
    return res;
}
}
