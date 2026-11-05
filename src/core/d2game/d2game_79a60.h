// Unit hand-over step: ECX = game, EDX = unit, two stack words that are never read (ret 8). Three steps in order: the first with
// ECX = game, EDX = unit and the game as its stack word; the second with EAX = unit, ECX = game; the third with ECX = game,
// EDX = unit, EAX = the game's frame counter (+0xA8) plus 0x1E and the stack words 0xB, that same value, 0, 0, 0.
#pragma once
#include <cstdint>

namespace d2g79a60 {
template <class E> void Run(E& e, uint32_t game, uint32_t unit)
{
    // 1.13c D2Game+0x79A60
    e.h(0x9C620, 0, game, unit, 0, 0, game);
    e.h(0x98250, unit, game);
    const uint32_t at = e.rd32(game + 0xA8) + 0x1E;
    e.h(0x8EA80, at, game, unit, 0, 0, 0xB, at, 0, 0, 0);
}
}
