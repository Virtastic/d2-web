// 1.13c D2Game+0xB2ED0: skill start callback of the monster inferno (table entry): the shared inferno start
// (d2game_inferno.h) with the skill record's calc at +0x13C.
#pragma once
#include "d2game_inferno.h"

namespace gb2ed0 {
template <class E> uint32_t Start(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t skillId, uint32_t level)
{
    return D2GameInfernoStart(e, dt, 0x13C, game, unit, skillId, level);
}
}
