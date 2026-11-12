// 1.13c D2Game+0x3BC80: owned monster skill callback (table entry). ecx = game, edx = the unit, one stack word
// (ret 4); the result is always 1. With a zero stack word, an owned unit and its owner (0xE03A0), the owner's
// entry for the unit's value (0xA3F4: ecx = owner, edx = 0xB26A of the unit) is looked up; when there is one its
// value (0xAD12) goes in eax, with the unit's room (0xA478) in ecx, to 0x47CF0 (game, unit x, unit y, 1).
#pragma once
#include <cstdint>
#include "d2game_monrec.h"

namespace g3bc80 {
template <class E> uint32_t Do(E& e, uint32_t game, uint32_t unit, uint32_t s0)
{
    if (s0 || !unit || !D2GameOwnedFlag(e, unit)) return 1;
    const uint32_t owner = D2GameOwner(e, game, unit);
    if (!owner) return 1;
    const uint32_t t = e.h(0xA3F4, 0, owner, e.h(0xB26A, 0, 0, 0, 0, 0, unit));
    if (!t) return 1;
    const uint32_t v = e.h(0xAD12, 0, 0, 0, 0, 0, t);
    const uint32_t y = e.h(0x1130, 0, unit);
    const uint32_t x = e.h(0x1100, 0, unit);
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    e.h(0x47CF0, v, room, 0, 0, 0, game, x, y, 1);
    return 1;
}
}
