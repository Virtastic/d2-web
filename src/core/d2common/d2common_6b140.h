// 1.13c D2Common+0x6B140: calc callback "random in range" (table entry). ecx = low, edx = high, second stack word =
// the owner record (first unread), ret 8. No record: 0. Otherwise the range roll (0x1180: eax = high, ecx = the seed
// 0x20 into the record itself, edi = low) and its result.
#pragma once
#include <cstdint>

namespace c6b140 {
template <class E> uint32_t RandomRange(E& e, uint32_t lo, uint32_t hi, uint32_t rec)
{
    if (!rec) return 0;
    const uint32_t r = e.h(0x1180, hi, rec + 0x20);
    e.j(0x1180, lo);
    return r;
}
}
