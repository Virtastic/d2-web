// Guarded unit hand-off: EAX = target unit, ECX = source, one stack word (ret 4). With both present and bit 26 of the target's
// flag word (+0xC4) clear, the hand-off step runs with the stack word, the source and the target as its three stack words
// (EAX = the stack word, ECX = the source at the call). Otherwise nothing happens.
#pragma once
#include <cstdint>

namespace d2g777f0 {
template <class E> void Run(E& e, uint32_t target, uint32_t source, uint32_t arg)
{
    // 1.13c D2Game+0x777F0
    if (!source || !target) return;
    if ((e.rd32(target + 0xC4) >> 26) & 1) return;
    e.h(0xDEA00, arg, source, 0, 0, 0, arg, source, target);
}
}
