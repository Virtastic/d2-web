// 1.13c D2Game+0x33260: object populate callback "corpse" (table entry). ecx and edx plus three stack words (ret
// 0xC) go unchanged to the common object populate (0x32CE0). When that made an object, flies are spawned on it
// (0x30E50: ecx = the first argument, esi = the object). The result is the object, or 0.
#pragma once
#include <cstdint>

namespace g33260 {
template <class E> uint32_t PopulateCorpse(E& e, uint32_t a, uint32_t b, uint32_t s0, uint32_t s1, uint32_t s2)
{
    const uint32_t obj = e.h(0x32CE0, 0, a, b, 0, 0, s0, s1, s2);
    if (obj) e.h(0x30E50, 0, a, 0, 0, obj);
    return obj;
}
}
