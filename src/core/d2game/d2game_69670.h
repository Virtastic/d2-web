// 1.13c D2Game+0x69670: shrine callback "health refill" (table entry). edx = the unit, one stack word = the amount
// (ret 4). The unit's maximum life (0xA3A0: unit) plus the amount shifted up eight bits is written to stats 6 and
// 7 (0xA472: unit, stat, value, 0). The result is 1.
#pragma once
#include <cstdint>

namespace g69670 {
template <class E> uint32_t Refill(E& e, uint32_t unit, uint32_t amount)
{
    const uint32_t v = e.h(0xA3A0, 0, 0, 0, 0, 0, unit) + (amount << 8);
    e.h(0xA472, 0, 0, 0, 0, 0, unit, 6, v, 0);
    e.h(0xA472, 0, 0, 0, 0, 0, unit, 7, v, 0);
    return 1;
}
}
