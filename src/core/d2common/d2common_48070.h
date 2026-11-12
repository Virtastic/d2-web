// 1.13c D2Common+0x48070: item property callback "add a stat, then reset durability to the maximum" (table
// entry). ecx = the mode word, edx = the second register word, nine stack words (ret 0x24): the item, the property
// record, words 2 and 3 (3 = the stat), ..., words 6 and 7 passed on. No record or no item: 0. An item unit
// (type 4) in mode 1 first clears the stat (0x46E50: esi = item; stack: the stat as a signed 16-bit word). The
// value is rolled (0x465B0: eax = record +0x0C, ecx = record +0x08, edx = item) and applied (0x475E0: eax =
// record; stack: edx word, item, words 2 and 3, 0, value, words 6 and 7). A zero result ends with 0. Otherwise the
// item's maximum durability is read (0x38FF0: item) and, when positive, stat 0x48 is set to it (0x3A740: item,
// 0x48, value, 0). The result is the apply call's result.
#pragma once
#include <cstdint>

namespace c48070 {
template <class E> uint32_t Apply(E& e, uint32_t c, uint32_t d, uint32_t item, uint32_t rec, uint32_t s2, uint32_t s3, uint32_t s6, uint32_t s7)
{
    if (!rec || !item) return 0;
    if (e.rd32(item) == 4 && c == 1) e.h(0x46E50, 0, 0, 0, 0, item, (uint32_t)(int32_t)(int16_t)s3);
    const uint32_t val = e.h(0x465B0, e.rd32(rec + 0xC), e.rd32(rec + 8), item);
    const uint32_t res = e.h(0x475E0, rec, 0, 0, 0, 0, d, item, s2, s3);
    const uint32_t w[4] = { 0, val, s6, s7 };
    e.x(0x475E0, w, 4);
    if (!res) return 0;
    const int32_t dur = (int32_t)e.h(0x38FF0, 0, 0, 0, 0, 0, item);
    if (dur > 0) e.h(0x3A740, 0, 0, 0, 0, 0, item, 0x48, (uint32_t)dur, 0);
    return res;
}
}
