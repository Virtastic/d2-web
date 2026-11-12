// 1.13c D2Game+0x69710: shrine effect callback (table entry). edx = the player, one stack word = the amount
// (ret 4). Adds the amount to the player's base stat 1 (0xA4D2: unit, stat, amount, layer 0). Result 1.
#pragma once
#include <cstdint>

namespace g69710 {
template <class E> uint32_t AddStat(E& e, uint32_t player, uint32_t amount)
{
    e.h(0xA4D2, 0, 0, 0, 0, 0, player, 1, amount, 0);
    return 1;
}
}
