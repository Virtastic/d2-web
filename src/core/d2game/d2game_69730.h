// 1.13c D2Game+0x69730: shrine callback "mana refill" (table entry). edx = the unit, one stack word = the amount
// (ret 4). The unit's maximum mana (0xA4EA: unit) plus the amount shifted up eight bits is written to stats 8 and
// 9 (0xA472: unit, stat, value, 0). The result is 1.
#pragma once
#include <cstdint>

namespace g69730 {
template <class E> uint32_t Refill(E& e, uint32_t unit, uint32_t amount)
{
    const uint32_t v = e.h(0xA4EA, 0, 0, 0, 0, 0, unit) + (amount << 8);
    e.h(0xA472, 0, 0, 0, 0, 0, unit, 8, v, 0);
    e.h(0xA472, 0, 0, 0, 0, 0, unit, 9, v, 0);
    return 1;
}
}
