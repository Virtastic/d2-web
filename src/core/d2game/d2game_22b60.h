// 1.13c D2Game+0x22B60: unique-monster modifier callback "random elemental damage" (table entry). ecx as
// entered, edx = the unit, stack: two words (ret 8). A zero second word: 0. Otherwise the unit's seed (+0x20
// low, +0x24 high) steps once (low * 0x6AC690C5 + high, 64 bits) and the new low word modulo 5 picks one of five
// stat triples, which go with the unit to the damage helper (0x21FE0: edx = ecx as entered; stack: unit, three
// stat ids); its result is the result.
#pragma once
#include <cstdint>

namespace g22b60 {
template <class E> uint32_t Apply(E& e, uint32_t ecx, uint32_t unit, uint32_t /*a1*/, uint32_t a2)
{
    static const uint32_t kStats[5][3] = {
        { 0x30, 0x31, 0xFFFFFFFFu }, { 0x32, 0x33, 0xFFFFFFFFu }, { 0x34, 0x35, 0xFFFFFFFFu }, { 0x36, 0x37, 0x38 }, { 0x39, 0x3A, 0x3B },
    };
    if (!a2) return 0;
    const uint64_t v = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5u + e.rd32(unit + 0x24);
    e.wr32(unit + 0x24, (uint32_t)(v >> 32));
    e.wr32(unit + 0x20, (uint32_t)v);
    const uint32_t* t = kStats[(uint32_t)v % 5];
    return e.h(0x21FE0, 0, 0, ecx, 0, 0, unit, t[0], t[1], t[2]);
}
}
