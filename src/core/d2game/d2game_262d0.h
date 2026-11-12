// 1.13c D2Game+0x262D0: unique-monster death callback "cold missile ring" (table entry). ecx = game, edx = the
// monster, two stack words (ret 8). A zero second word does nothing and gives 0. Otherwise half the monster's
// level (0xA42A: monster, 12, 0; signed, rounded toward zero, at least 1) and the missile id (0xACFA: 0x77, 0)
// feed the ring cast (0xA2800: eax = monster, ebx = missile id; stack: game, monster, 0xC2, 0, half level); that
// call's result is the result.
#pragma once
#include <cstdint>

namespace g262d0 {
template <class E> uint32_t Death(E& e, uint32_t game, uint32_t unit, uint32_t b)
{
    if (!b) return 0;
    int32_t n = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, unit, 12, 0) / 2;
    if (n <= 1) n = 1;
    const uint32_t id = e.h(0xACFA, 0, 0, 0, 0, 0, 0x77, 0);
    return e.h(0xA2800, unit, 0, 0, id, 0, game, unit, 0xC2, 0, (uint32_t)n);
}
}
