// 1.13c D2Game+0xB31B0: skill start callback (table entry). edx = the unit, two stack words unread (ret 8).
// Clears the three flag bits 0xE of the unit's flag word (+0xC4) when there is a unit, then marks the unit
// (0xACA6: unit, 1). Result 1.
#pragma once
#include <cstdint>

namespace gb31b0 {
template <class E> uint32_t Start(E& e, uint32_t unit)
{
    if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) & ~0xEu);
    e.h(0xACA6, 0, 0, 0, 0, 0, unit, 1);
    return 1;
}
}
