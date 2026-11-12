// 1.13c D2Game+0x3EA60: owned monster skill callback (table entry). ecx = game, edx = the unit, one stack word
// (ret 4). A null unit, a unit without the owner flag or without an owner (0xE03A0) gives 1. Otherwise, when the
// stack word is set and the owner test (0xE0570: eax = the word, ecx = the owner) passes, the unit's two values
// (0xB0FC, 0xB26A) go to 0x29240 (game, owner, second, first). The result is then 0.
#pragma once
#include <cstdint>
#include "d2game_monrec.h"

namespace g3ea60 {
template <class E> uint32_t Do(E& e, uint32_t game, uint32_t unit, uint32_t s0)
{
    if (!unit || !D2GameOwnedFlag(e, unit)) return 1;
    const uint32_t owner = D2GameOwner(e, game, unit);
    if (!owner) return 1;
    if (s0 && e.h(0xE0570, s0, owner)) {
        const uint32_t a = e.h(0xB0FC, 0, 0, 0, 0, 0, unit);
        const uint32_t b = e.h(0xB26A, 0, 0, 0, 0, 0, unit);
        e.h(0x29240, 0, 0, 0, 0, 0, game, owner, b, a);
    }
    return 0;
}
}
