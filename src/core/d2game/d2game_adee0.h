// 1.13c D2Game+0xADEE0: item spell callback "cast at coordinates" (table entry). edx = the caster, five stack words
// (ret 0x14): the item, one unread, x, y, one unread. The item's class (+0x04; -1 without an item) is looked up
// (0xA53E: class); no record ends with 0. Otherwise the cast runs with the low words of x and y (0xF1730: eax =
// caster, ebx = y; stack: 0x2F, 1, x, 0) and its result is the result.
#pragma once
#include <cstdint>

namespace gadee0 {
template <class E> uint32_t Cast(E& e, uint32_t unit, uint32_t item, uint32_t x, uint32_t y)
{
    const uint32_t r = e.h(0xA53E, 0, 0, 0, 0, 0, item ? e.rd32(item + 4) : 0xFFFFFFFFu);
    if (!r) return 0;
    return e.h(0xF1730, unit, 0, 0, y & 0xFFFF, 0, 0x2F, 1, x & 0xFFFF, 0);
}
}
