// 1.13c D2Game+0x21BB0: unique-monster mode-change callback (table entry). ecx = game, edx = the monster, two stack
// words (ret 8); the second is a flag. Flag clear: 0. A monster in a mode other than 0: its mode comes back.
// Otherwise (no monster, or mode 0) an event of type 7 is queued four frames ahead (0x8EA80: game, monster, 7,
// frame + 4, 0, 0, 0) and that call's result is the result.
#pragma once
#include <cstdint>

namespace g21bb0 {
template <class E> uint32_t ModeChange(E& e, uint32_t game, uint32_t unit, uint32_t flag)
{
    if (!flag) return 0;
    if (unit) {
        const uint32_t mode = e.rd32(unit + 0x10);
        if (mode) return mode;
    }
    return e.h(0x8EA80, 0, game, unit, 0, 0, 7, e.rd32(game + 0xA8) + 4, 0, 0, 0);
}
}
