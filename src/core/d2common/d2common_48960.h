// 1.13c D2Common+0x48960: item property callback "add a stat, clearing it first on an item" (table entry). ecx
// unread, edx = the second register word, nine stack words (ret 0x24): the item, the property record, words 2 and
// 3 (3 = the stat), one unread, a preset value, words 6 and 7 passed on. No record: 0. An item unit (type 4)
// first clears the stat (0x46E50: esi = item; stack: the stat as a signed 16-bit word). A zero preset value is
// replaced by a roll (0x465B0: eax = record +0x0C, ecx = record +0x08, edx = item). The stat is applied (0x475E0:
// eax = record; stack: edx word, item, words 2 and 3, 0, the value, words 6 and 7); that call's result is the
// result.
#pragma once
#include <cstdint>

namespace c48960 {
template <class E> uint32_t Apply(E& e, uint32_t d, uint32_t item, uint32_t rec, uint32_t s2, uint32_t s3, uint32_t val, uint32_t s6, uint32_t s7)
{
    if (!rec) return 0;
    if (item && e.rd32(item) == 4) e.h(0x46E50, 0, 0, 0, 0, item, (uint32_t)(int32_t)(int16_t)s3);
    if (!val) val = e.h(0x465B0, e.rd32(rec + 0xC), e.rd32(rec + 8), item);
    const uint32_t res = e.h(0x475E0, rec, 0, 0, 0, 0, d, item, s2, s3);
    const uint32_t w[4] = { 0, val, s6, s7 };
    e.x(0x475E0, w, 4);
    return res;
}
}
