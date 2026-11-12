// 1.13c D2Game+0x22880: monster callback that sets stat 0x43 from the monster record (table entry). eax = the
// entry value (returned untouched when nothing is done), ecx = the unit, one stack word unread (ret 4). The unit
// must be a monster (type 1) with a non-negative class (+0x04), which is returned as is when it is negative or
// outside the monster table (data tables +0xA80 records of 0x1A8 bytes at +0xA78). The signed word at record
// +0x32 must be positive (otherwise eax holds the record address with that word in its low half). The value is
// 0x800 / word - 0x80 limited to 10..100 and the result is that of the stat write (0xA4D2: unit, 0x43, value, 0).
#pragma once
#include <cstdint>

namespace g22880 {
template <class E> uint32_t Apply(E& e, uint32_t dt, uint32_t eax0, uint32_t unit)
{
    if (!unit || e.rd32(unit) != 1) return eax0;
    const int32_t cls = (int32_t)e.rd32(unit + 4);
    if (cls < 0) return (uint32_t)cls;
    if (cls >= (int32_t)e.rd32(dt + 0xA80)) return (uint32_t)cls;
    const uint32_t rec = e.rd32(dt + 0xA78) + (uint32_t)cls * 0x1A8;
    if (!rec) return 0;
    const int32_t w = e.rd16s(rec + 0x32);
    if (w <= 0) return (rec & 0xFFFF0000u) | (uint16_t)w;
    int32_t v = 0x800 / w - 0x80;
    if (v <= 10) v = 10;
    else if (v >= 100) v = 100;
    return e.h(0xA4D2, 0, 0, 0, 0, 0, unit, 0x43, (uint32_t)v, 0);
}
}
