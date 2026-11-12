// 1.13c D2Common+0x485D0: item property callback "add a stat" (table entry). ecx unread, edx = the second
// register word, nine stack words (ret 0x24): the item, the property record, two words passed on, one unread, a
// preset value, two words passed on, one unread. No record: 0. A zero preset value is replaced by a roll
// (0x465B0: eax = record +0x0C, ecx = record +0x08, edx = item). The stat is then applied on layer 0 (0x475E0:
// eax = record; stack: edx word, item, words 3 and 4, 0, value, words 7 and 8) and that call's result is the
// result.
#pragma once
#include <cstdint>

namespace c485d0 {
template <class E> uint32_t Apply(E& e, uint32_t d, uint32_t item, uint32_t rec, uint32_t s2, uint32_t s3, uint32_t val, uint32_t s6, uint32_t s7)
{
    if (!rec) return 0;
    if (!val) val = e.h(0x465B0, e.rd32(rec + 0xC), e.rd32(rec + 8), item);
    return e.h(0x475E0, rec, 0, 0, 0, 0, d, item, s2, s3, 0, val, s6, s7);
}
}
