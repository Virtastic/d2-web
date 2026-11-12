// 1.13c D2Game+0x21B00: unique-monster mode-change callback (table entry). ecx = game, edx = the monster, two stack
// words unread (ret 8). A monster in a mode other than 0 and 3 is left alone and its mode comes back. Otherwise
// (no monster, mode 0 or mode 3) an event of type 7 is queued two frames ahead (0x8EA80: game, monster, 7,
// frame + 2, 0, 0, 0) and that call's result is the result.
#pragma once
#include <cstdint>

namespace g21b00 {
template <class E> uint32_t ModeChange(E& e, uint32_t game, uint32_t unit)
{
    if (unit) {
        const uint32_t mode = e.rd32(unit + 0x10);
        if (mode != 3 && mode != 0) return mode;
    }
    return e.h(0x8EA80, 0, game, unit, 0, 0, 7, e.rd32(game + 0xA8) + 2, 0, 0, 0);
}
}
