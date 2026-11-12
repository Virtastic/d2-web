// 1.13c D2Game+0x3F540: missile per-frame callback (table entry). ecx = game, edx = the missile. Calls the basic
// missile step (0x3F320: ebx = missile, one stack word = game); its result is the result.
#pragma once
#include <cstdint>

namespace g3f540 {
template <class E> uint32_t BasicMissile(E& e, uint32_t game, uint32_t missile)
{
    return e.h(0x3F320, 0, game, missile, missile, 0, game);
}
}
