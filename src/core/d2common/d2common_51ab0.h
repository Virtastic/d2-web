// 1.13c D2Common+0x51AB0: chained skill-description calc (table entry). ecx and edx as entered, stack: one word and
// a pointer to three words (ret 8). No pointer: 0. Otherwise the evaluator (0x51070, four stack words) runs twice:
// first on (word 0, edx, word 1, word 2), then on (word 0, the stack word, ecx, the first result); the second
// result is the result.
#pragma once
#include <cstdint>

namespace c51ab0 {
template <class E> uint32_t Chain(E& e, uint32_t ecx, uint32_t edx, uint32_t a, uint32_t p)
{
    if (!p) return 0;
    const uint32_t inner = e.h(0x51070, 0, 0, 0, 0, 0, e.rd32(p), edx, e.rd32(p + 4), e.rd32(p + 8));
    return e.h(0x51070, 0, 0, 0, 0, 0, e.rd32(p), a, ecx, inner);
}
}
