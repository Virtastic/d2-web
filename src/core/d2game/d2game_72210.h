// 1.13c D2Game+0x72210: cube output callback (table entry). ecx = game, edx = the player. Calls the quest-item
// step (0x674A0, two stack words: game, player); its result is the result.
#pragma once
#include <cstdint>

namespace g72210 {
template <class E> uint32_t QuestItemStep(E& e, uint32_t game, uint32_t player)
{
    return e.h(0x674A0, 0, game, player, 0, 0, game, player);
}
}
