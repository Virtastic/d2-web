// 1.13c D2Game+0x754E0: monster mode-start callback "sequence" (table entry). ecx = game, edx = the mode block
// (+0x04 the monster). Drops the attacker list (0xDA750: one stack word = game, same registers), puts the monster
// in animation mode 14 (0xA4A2: unit, mode), clears bit 0x40 of the unit flags (+0xC4) when there is a unit, then
// runs the skill start (0xA0830: one stack word = game, esi and edi = the monster); that call's result is the
// result.
#pragma once
#include <cstdint>

namespace g754e0 {
template <class E> uint32_t Start(E& e, uint32_t game, uint32_t blk)
{
    const uint32_t unit = e.rd32(blk + 4);
    e.h(0xDA750, 0, game, blk, 0, 0, game);
    e.h(0xA4A2, 0, 0, 0, 0, 0, unit, 14);
    if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) & ~0x40u);
    const uint32_t r = e.h(0xA0830, 0, 0, 0, 0, unit, game);
    e.j(0xA0830, unit);
    return r;
}
}
