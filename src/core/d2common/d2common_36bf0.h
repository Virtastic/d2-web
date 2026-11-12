// 1.13c D2Common+0x36BF0: toggle bits. ecx = the flag word's address, edx = the bits; the word is xor-ed in place.
// EAX is not touched: the caller's value comes back unchanged.
#pragma once
#include <cstdint>

namespace c36bf0 {
template <class E> uint32_t Toggle(E& e, uint32_t eaxIn, uint32_t word, uint32_t bits)
{
    e.wr32(word, e.rd32(word) ^ bits);
    return eaxIn;
}
}
