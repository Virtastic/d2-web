// 1.13c D2Game+0xAC8E0: monster think callback (table entry). ecx = game, edx = the monster, one stack word = the
// think parameters (ret 4). A monster of class 420 (+0x04) takes its own think (0xAAC30: eax = the parameters, ecx
// and edx = the monster, one stack word = game). Any other, or none, takes the common pet think (0xAA710: eax and
// edx = the monster, ecx = game, stack: game, parameters). The call's result is the result.
#pragma once
#include <cstdint>

namespace gac8e0 {
template <class E> uint32_t Think(E& e, uint32_t game, uint32_t unit, uint32_t prm)
{
    if (unit && e.rd32(unit + 4) == 0x1A4) return e.h(0xAAC30, prm, unit, unit, 0, 0, game);
    return e.h(0xAA710, unit, game, unit, 0, 0, game, prm);
}
}
