// 1.13c D2Game+0x1BC60: AI think callback for towners (table entry). ecx = game, edx = the unit, one stack word
// (ret 4). Three steps are tried in order and the first whose low result byte is non-zero ends the think with that
// result: 0x15D10 (eax = unit; game), 0x16A30 (eax = unit; game), 0x1BBC0 (game, unit, the stack word). When none
// fires the unit idles (0xF22C0: eax = game; 12) and that call's result is the result.
#pragma once
#include <cstdint>

namespace g1bc60 {
template <class E> uint32_t Think(E& e, uint32_t game, uint32_t unit, uint32_t a)
{
    uint32_t r = e.h(0x15D10, unit, 0, 0, 0, 0, game);
    if (r & 0xFF) return r;
    r = e.h(0x16A30, unit, 0, 0, 0, 0, game);
    if (r & 0xFF) return r;
    r = e.h(0x1BBC0, 0, 0, 0, 0, 0, game, unit, a);
    if (r & 0xFF) return r;
    return e.h(0xF22C0, game, 0, 0, 0, 0, 12);
}
}
