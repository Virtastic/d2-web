// 1.13c D2Common+0x48470: item property callback "add a class skill bonus" (table entry). ecx unread, edx = the
// second register word, nine stack words (ret 0x24): the item, the property record, words 2 and 3, one unread,
// a preset value, words 6 and 7 passed on. No record: 0. A zero preset value is replaced by a roll (0x465B0: eax =
// record +0x0C, ecx = record +0x08, edx = item); a zero roll ends with 0. The layer is the record's +0x04 word
// split by three: remainder plus eight times the quotient (signed division). The stat is applied (0x475E0: eax =
// record; stack: edx word, item, words 2 and 3, the layer, the value, words 6 and 7); that call's result is the
// result.
#pragma once
#include <cstdint>

namespace c48470 {
template <class E> uint32_t Apply(E& e, uint32_t d, uint32_t item, uint32_t rec, uint32_t s2, uint32_t s3, uint32_t val, uint32_t s6, uint32_t s7)
{
    if (!rec) return 0;
    if (!val) { val = e.h(0x465B0, e.rd32(rec + 0xC), e.rd32(rec + 8), item); if (!val) return 0; }
    const int32_t p = (int32_t)e.rd32(rec + 4);
    const uint32_t layer = (uint32_t)(p % 3) + (uint32_t)(p / 3) * 8;
    const uint32_t res = e.h(0x475E0, rec, 0, 0, 0, 0, d, item, s2, s3);
    const uint32_t w[4] = { layer, val, s6, s7 };
    e.x(0x475E0, w, 4);
    return res;
}
}
