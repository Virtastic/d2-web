// 1.13c D2Game+0x7C8C0: AI think callback "clear the channel state and reset the AI" (table entry). ecx = game,
// edx = the monster, one stack word (ret 4): the AI tick block, whose +0x00 points at the AI control. A monster
// carrying state 12 (0xA68E: monster, 12) has it cleared (0xA3CA: monster, 12, 0). The AI function is then re-run
// (0x9D230: ecx = game, edx = monster; stack: the AI control, its first word) and the last call (0xF22C0: eax =
// game; stack: 1) gives the result.
#pragma once
#include <cstdint>

namespace g7c8c0 {
template <class E> uint32_t Think(E& e, uint32_t game, uint32_t unit, uint32_t tick)
{
    const uint32_t ai = e.rd32(tick);
    if (e.h(0xA68E, 0, 0, 0, 0, 0, unit, 12)) e.h(0xA3CA, 0, 0, 0, 0, 0, unit, 12, 0);
    const uint32_t first = e.rd32(ai);
    e.h(0x9D230, 0, game, unit, 0, 0, e.rd32(tick), first);
    return e.h(0xF22C0, game, 0, 0, 0, 0, 1);
}
}
