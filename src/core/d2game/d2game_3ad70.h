// 1.13c D2Game+0x3AD70: missile target callback "player or allied monster" (table entry). ecx = the candidate
// unit, edx = the argument block (four words; the last must be non-zero). The candidate must be a player (type 0
// at +0x00) or a monster (type 1) whose alignment (0xA514: unit) is 2; no candidate counts as neither. Then the
// range helper decides (0x3AA20: ebx = block word 3, esi = candidate, edi = block word 2; stack: block words 0
// and 1): the result is 1 when it answers non-zero, else 0.
#pragma once
#include <cstdint>

namespace g3ad70 {
template <class E> uint32_t Test(E& e, uint32_t unit, uint32_t p)
{
    if (!p || !e.rd32(p + 0xC)) return 0;
    const uint32_t type = unit ? e.rd32(unit) : 6;
    if (type == 1) { if (e.h(0xA514, 0, 0, 0, 0, 0, unit) != 2) return 0; }
    else if (type != 0) return 0;
    const uint32_t edi = e.rd32(p + 8);
    const uint32_t r = e.h(0x3AA20, 0, 0, 0, e.rd32(p + 0xC), unit, e.rd32(p), e.rd32(p + 4));
    e.j(0x3AA20, edi);
    return r ? 1 : 0;
}
}
