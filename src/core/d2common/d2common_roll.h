// 1.13c D2Common: the inlined "roll between the record's two bounds" block shared by the item property callbacks
// at 0x48100, 0x48190, 0x48220, 0x482B0 and 0x48AE0. The bounds are record +0x08 and +0x0C. Equal bounds give that
// value with no call. Otherwise the larger is passed in eax and the smaller in edi to the ranged roll (0x1180),
// with ecx = the seed: for an item unit (type 4) its data pointer (+0x14) plus 4, or null when it has none; for
// anything else (a null item included) the unit's own seed at +0x20.
#pragma once
#include <cstdint>

template <class E> uint32_t D2CommonPropRoll(E& e, uint32_t item, uint32_t rec)
{
    int32_t hi = (int32_t)e.rd32(rec + 0xC), lo = (int32_t)e.rd32(rec + 8);
    if (lo == hi) return (uint32_t)lo;
    if (hi < lo) { const int32_t t = hi; hi = lo; lo = t; }
    uint32_t seed;
    if (item && e.rd32(item) == 4) { const uint32_t data = e.rd32(item + 0x14); seed = data ? data + 4 : 0; }
    else seed = item + 0x20;
    const uint32_t v = e.h(0x1180, (uint32_t)hi, seed);
    e.j(0x1180, (uint32_t)lo);
    return v;
}
