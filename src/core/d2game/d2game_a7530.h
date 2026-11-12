// 1.13c D2Game+0xA7530: AI think callback "Ancient" (table entry). ecx = game, edx = the monster, one stack word
// (ret 4): the AI tick block. The monster's class (+0x04; -1 without a monster) picks one of three routines:
// 0x21C (0xA62D0: eax = tick block, ecx = monster; stack: game), 0x21D (0xA6050: eax = tick block; stack: game,
// monster) or 0x21E (0xA5EB0: eax = tick block, ecx = monster; stack: game); the routine's result is the result.
// Any other class leaves the class minus 0x21E.
#pragma once
#include <cstdint>

namespace ga7530 {
template <class E> uint32_t Think(E& e, uint32_t game, uint32_t unit, uint32_t tick)
{
    const uint32_t cls = unit ? e.rd32(unit + 4) : 0xFFFFFFFFu;
    if (cls == 0x21C) return e.h(0xA62D0, tick, unit, 0, 0, 0, game);
    if (cls == 0x21D) return e.h(0xA6050, tick, 0, 0, 0, 0, game, unit);
    if (cls == 0x21E) return e.h(0xA5EB0, tick, unit, 0, 0, 0, game);
    return cls - 0x21E;
}
}
