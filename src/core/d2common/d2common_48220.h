// 1.13c D2Common+0x48220: item property callback "add a stat, the roll as the layer and stack word 4 as the
// value" (table entry). ecx unread, edx = the second register word, nine stack words (ret 0x24): the item, the
// property record, words 2, 3 and 4, ..., words 6 and 7 passed on. No record: 0. A value is rolled between the
// record's bounds (d2common_roll.h) and the stat applied (0x475E0: eax = record; stack: edx word, item, words 2
// and 3, the roll, word 4 as a signed 16-bit value, words 6 and 7); that call's result is the result.
#pragma once
#include <cstdint>
#include "d2common_roll.h"

namespace c48220 {
template <class E> uint32_t Apply(E& e, uint32_t d, uint32_t item, uint32_t rec, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s6, uint32_t s7)
{
    if (!rec) return 0;
    const uint32_t roll = D2CommonPropRoll(e, item, rec);
    const uint32_t res = e.h(0x475E0, rec, 0, 0, 0, 0, d, item, s2, s3);
    const uint32_t w[4] = { roll, (uint32_t)(int32_t)(int16_t)s4, s6, s7 };
    e.x(0x475E0, w, 4);
    return res;
}
}
