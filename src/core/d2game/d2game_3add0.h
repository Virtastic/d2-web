// 1.13c D2Game+0x3ADD0: missile do-function target filter "monster only" (table entry). ecx = the candidate unit,
// edx = the argument block (four words). A null block, a zero fourth word, a null unit or a unit type other than 1
// gives 0. Otherwise the shared test runs (0x3AA20: ebx = word 3, esi = the unit, edi = word 2; stack: word 0,
// word 1) and the result is 1 when it is non-zero, else 0.
#pragma once
#include <cstdint>

namespace g3add0 {
template <class E> uint32_t Filter(E& e, uint32_t unit, uint32_t p)
{
    if (!p) return 0;
    const uint32_t w3 = e.rd32(p + 0xC);
    if (!w3 || !unit || e.rd32(unit) != 1) return 0;
    const uint32_t r = e.h(0x3AA20, 0, 0, 0, w3, unit, e.rd32(p), e.rd32(p + 4));
    e.j(0x3AA20, e.rd32(p + 8));
    return r ? 1 : 0;
}
}
