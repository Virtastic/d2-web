// 1.13c D2Game+0x79170: unit mode callback (table entry). ecx = game, edx = unit, stack: mode, a second word.
// Without the unit's path (0xAABA: unit) the answer is 2. The second stack word is stored (0xACE8: unit, word;
// retail reads [esp+0x14] before its fourth push, which is that word, not the mode)
// and the path's flags are read (0xA79C: path). With flag bit 0: 0xE07A0 (eax = unit, ecx = game) and, when
// 0xE1930 (eax = unit) answers 2, the flags get bit 1 (0xA78A: path, flags) and the game is told (0xA2060:
// game). Otherwise the game is told only for modes 1 and 2 of a unit without bit 6 in its flag word +0xC4.
// The answer is 2 for a dead unit (0xDFA90: eax = unit), else 1.
#pragma once
#include <cstdint>

namespace g79170 {
template <class E> uint32_t Mode(E& e, uint32_t game, uint32_t unit, uint32_t mode, uint32_t word)
{
    const uint32_t path = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!path) return 2;
    e.h(0xACE8, 0, 0, 0, 0, 0, unit, word);
    const uint32_t f = e.h(0xA79C, 0, 0, 0, 0, 0, path);
    bool tell = false, done = false;
    if (f & 1) {
        e.h(0xE07A0, unit, game);
        if (e.h(0xE1930, unit) == 2) {
            e.h(0xA78A, 0, 0, 0, 0, 0, path, f | 2);
            tell = done = true;
        }
    }
    if (!done && !(unit && ((e.rd32(unit + 0xC4) >> 6) & 1))) tell = mode == 1 || mode == 2;
    if (tell) e.h(0xA2060, 0, 0, 0, 0, 0, game);
    return (e.h(0xDFA90, unit) != 0) + 1;
}
}
