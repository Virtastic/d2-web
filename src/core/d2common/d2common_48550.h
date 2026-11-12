// 1.13c D2Common+0x48550: item property callback "add a single skill bonus" (table entry). ecx unread, edx = the
// second register word, nine stack words (ret 0x24): the item, the property record, words 2 and 3, one unread,
// a preset value, words 6 and 7 passed on. No record: 0. A zero preset value is replaced by a roll (0x465B0: eax =
// record +0x0C, ecx = record +0x08, edx = item); a zero roll ends with 0. The skill id (record +0x04) must be in
// 0 .. the skill count (data tables +0xBA0) - 1, else 0. The stat is applied (0x475E0: eax = record; stack: edx
// word, item, words 2 and 3, the skill id as the layer, the value, words 6 and 7); that call's result is the
// result.
#pragma once
#include <cstdint>

namespace c48550 {
template <class E> uint32_t Apply(E& e, uint32_t dt, uint32_t d, uint32_t item, uint32_t rec, uint32_t s2, uint32_t s3, uint32_t val, uint32_t s6, uint32_t s7)
{
    if (!rec) return 0;
    if (!val) { val = e.h(0x465B0, e.rd32(rec + 0xC), e.rd32(rec + 8), item); if (!val) return 0; }
    const int32_t skill = (int32_t)e.rd32(rec + 4);
    if (skill < 0 || skill >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t res = e.h(0x475E0, rec, 0, 0, 0, 0, d, item, s2, s3);
    const uint32_t w[4] = { (uint32_t)skill, val, s6, s7 };
    e.x(0x475E0, w, 4);
    return res;
}
}
