// 1.13c D2Game: the inlined monster record lookup shared by the monster skill callbacks. The unit's class id
// (+4) selects a 0x1A4-byte record of the table at data tables +0xB64 (count at +0xB6C); a null unit or an id
// outside the table gives no record. The owner flag is bit 10 of the unit's word at +0xC8.
#pragma once
#include <cstdint>

template <class E> uint32_t D2GameMonRec(E& e, uint32_t dt, uint32_t unit)
{
    if (!unit) return 0;
    const int32_t id = (int32_t)e.rd32(unit + 4);
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xB6C)) return 0;
    return e.rd32(dt + 0xB64) + (uint32_t)id * 0x1A4;
}
template <class E> bool D2GameOwnedFlag(E& e, uint32_t unit) { return (e.rd32(unit + 0xC8) >> 10) & 1; }
template <class E> uint32_t D2GameOwner(E& e, uint32_t game, uint32_t unit)
{
    return e.h(0xE03A0, e.rd32(unit + 0x94), game, e.rd32(unit + 0x98));
}
