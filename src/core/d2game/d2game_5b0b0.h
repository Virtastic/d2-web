// 1.13c D2Game+0x5B0B0 ACT5Q3_ApplyResistanceReward: EAX = player unit, stack word = quest arg handed on (ret 4); void.
// Three times: the player's quest-flag record (Ordinal 10920, pdata+0x10/+0x14/+0x18 for the three difficulties) is asked
// for flag (0x25, 7) by Ordinal 10174; each set flag adds 10 to a local total, and a nonzero total calls 0x5A000 with the
// quest arg (EAX also holds it). The total itself never leaves the function.
#pragma once
#include <cstdint>

namespace d2g5b0b0 {
template <class E> void Run(E& e, uint32_t unit, uint32_t arg)
{
    uint32_t sum = 0;
    for (uint32_t i = 0; i < 3; i++) {
        const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
        if (e.h(0xA568, 0, 0, 0, 0, 0, e.rd32(pd + 0x10 + 4 * i), 0x25, 7)) sum += 10;
    }
    if (sum) e.h(0x5A000, arg, 0, 0, 0, 0, arg);
}
}
