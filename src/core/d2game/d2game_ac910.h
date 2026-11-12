// 1.13c D2Game+0xAC910: item event callback (table entry). Five stack words (ret 0x14); only the first two are
// read. Both non-zero: state 172 is switched on for the second (0xA3CA: unit, 172, 1) and the result is 1.
// Otherwise 0.
#pragma once
#include <cstdint>

namespace gac910 {
template <class E> uint32_t Event(E& e, uint32_t s0, uint32_t s1)
{
    if (!s0 || !s1) return 0;
    e.h(0xA3CA, 0, 0, 0, 0, 0, s1, 0xAC, 1);
    return 1;
}
}
