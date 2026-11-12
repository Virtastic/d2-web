// 1.13c D2Game+0x269F0: unique-monster modifier callback (table entry). eax as entered, ecx = the monster, one
// stack word unread (ret 4). No unit or not a monster (type 1 at +0x00): eax is untouched. A monster without a
// game (+0x80): 0. Otherwise an event of type 7 is queued 75 frames ahead (0x8EA80: game, monster, 7, frame + 75,
// 0, 0, 0) and that call's result is the result.
#pragma once
#include <cstdint>

namespace g269f0 {
template <class E> uint32_t Mod(E& e, uint32_t eax, uint32_t unit)
{
    if (!unit || e.rd32(unit) != 1) return eax;
    const uint32_t game = e.rd32(unit + 0x80);
    if (!game) return 0;
    return e.h(0x8EA80, 0, game, unit, 0, 0, 7, e.rd32(game + 0xA8) + 0x4B, 0, 0, 0);
}
}
