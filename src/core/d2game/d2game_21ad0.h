// 1.13c D2Game+0x21AD0: unique-monster mode-change callback (table entry). ecx = game, edx = the monster, two stack
// words (ret 8); the second is a flag. Flag clear: 0. No monster, or a monster not in mode 3: the flag itself comes
// back. Otherwise an event of type 7 is queued two frames ahead (0x8EA80: game, monster, 7, frame + 2, 0, 0, 0) and
// that call's result is the result.
#pragma once
#include <cstdint>

namespace g21ad0 {
template <class E> uint32_t ModeChange(E& e, uint32_t game, uint32_t unit, uint32_t flag)
{
    if (!flag) return 0;
    if (!unit || e.rd32(unit + 0x10) != 3) return flag;
    return e.h(0x8EA80, 0, game, unit, 0, 0, 7, e.rd32(game + 0xA8) + 2, 0, 0, 0);
}
}
