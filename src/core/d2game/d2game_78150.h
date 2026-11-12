// 1.13c D2Game+0x78150: unit mode change callback (table entry). ecx = the game, edx = the unit, three stack
// words (ret 0xC) of which only the first, the mode, is read. A null unit is the fatal assert at line 0x46F.
// Otherwise the shared head (pending event dropped, mode set), the unit's path target cleared (0xA496: unit, 0)
// between the animation reset and the announcement, and the shared tail's result.
#pragma once
#include <cstdint>
#include "d2game_fatal.h"
#include "d2game_modeseq.h"

namespace g78150 {
template <class E> uint32_t ModeLine(E& e, uint32_t game, uint32_t unit, uint32_t mode, uint32_t line)
{
    if (!unit) d2gfatal::Fatal(e, line, 0);
    d2gmodeseq::Head(e, game, unit, mode);
    e.h(0xDFF10, 0, 0, 0, 0, unit);
    e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
    e.h(0xE0750, game, 0, 0, unit);
    return e.h(0xDF7B0, 0, 0, 0, 0, unit, game);
}
template <class E> uint32_t Mode(E& e, uint32_t game, uint32_t unit, uint32_t mode) { return ModeLine(e, game, unit, mode, 0x46F); }
}
