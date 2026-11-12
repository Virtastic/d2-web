// 1.13c D2Game+0x781C0: unit mode change callback (table entry), the same body as the one at 0x78150 with the
// null-unit assert at line 0x446. ecx = the game, edx = the unit, three stack words (ret 0xC), the first the mode.
#pragma once
#include <cstdint>
#include "d2game_78150.h"

namespace g781c0 {
template <class E> uint32_t Mode(E& e, uint32_t game, uint32_t unit, uint32_t mode) { return g78150::ModeLine(e, game, unit, mode, 0x446); }
}
