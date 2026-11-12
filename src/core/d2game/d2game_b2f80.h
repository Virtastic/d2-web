// 1.13c D2Game+0xB2F80: skill start callback (table entry). edx = unit, two stack words unread (ret 8). A unit
// present gets the three flag bits 0xE set in its flags word (+0xC4). Result 1.
#pragma once
#include <cstdint>

namespace gb2f80 {
template <class E> uint32_t Start(E& e, uint32_t unit)
{
    if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0xEu);
    return 1;
}
}
