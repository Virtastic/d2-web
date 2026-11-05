// Act V quest arm step for quest 0x24: the shared rule in d2game_5b120.h with its own callback.
#pragma once
#include "d2game_5b120.h"
namespace d2gArmQuestCb {
// 1.13c D2Game+0x5B180
template <class E> uint32_t Run5B180(E& e, uint32_t game, uint32_t unit, uint32_t mod) { return Arm(e, game, unit, 0x24, mod + 0x582E0); }
}
