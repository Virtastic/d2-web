// 1.13c D2Game+0x21A70: unique-monster modifier callback (table entry). edx = unit, two stack words unread (ret 8).
// A null unit or one in mode 0 (+0x10) gets state 0x6B switched on (0xA3CA: unit, 0x6B, 1), whose result is left in
// EAX; any other unit is left alone with its mode in EAX.
#pragma once
#include <cstdint>

namespace g21a70 {
template <class E> uint32_t Shatter(E& e, uint32_t unit)
{
    if (unit) { const uint32_t mode = e.rd32(unit + 0x10); if (mode) return mode; }
    return e.h(0xA3CA, 0, 0, 0, 0, 0, unit, 0x6B, 1);
}
}
