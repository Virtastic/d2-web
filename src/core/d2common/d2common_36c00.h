// 1.13c D2Common+0x36C00: keep bits. ecx = the flag word's address, edx = the mask; the word is and-ed in place.
// EAX is not touched: the caller's value comes back unchanged.
#pragma once
#include <cstdint>

namespace c36c00 {
template <class E> uint32_t Keep(E& e, uint32_t eaxIn, uint32_t word, uint32_t mask)
{
    e.wr32(word, e.rd32(word) & mask);
    return eaxIn;
}
}
