// 1.13c D2Common+0x50C90: skill-calc callback "missile parameter" (table entry). ecx = a, edx = b, second stack word
// = the calc context (first unread), ret 8. No context: 0. Otherwise the missile special-parameter lookup (0x6A790,
// five stack words: 0, context word 0, b, a, context word 2) and its result.
#pragma once
#include <cstdint>

namespace c50c90 {
template <class E> uint32_t MissileParam(E& e, uint32_t a, uint32_t b, uint32_t ctx)
{
    if (!ctx) return 0;
    const uint32_t w4[1] = { e.rd32(ctx + 8) };
    const uint32_t r = e.h(0x6A790, 0, 0, 0, 0, 0, 0, e.rd32(ctx), b, a);
    e.x(0x6A790, w4, 1);
    return r;
}
}
