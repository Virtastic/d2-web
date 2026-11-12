// 1.13c D2Common+0x50CB0: skill-calc callback "random in range" (table entry). ecx = low, edx = high, second stack
// word = the calc context (first unread), ret 8. No context: 0. Otherwise the range roll (0x1180: eax = high, ecx = the
// seed, which sits 0x20 into the record the context's first word points at, edi = low) and its result.
#pragma once
#include <cstdint>

namespace c50cb0 {
template <class E> uint32_t RandomRange(E& e, uint32_t lo, uint32_t hi, uint32_t ctx)
{
    if (!ctx) return 0;
    const uint32_t r = e.h(0x1180, hi, e.rd32(ctx) + 0x20);
    e.j(0x1180, lo);
    return r;
}
}
