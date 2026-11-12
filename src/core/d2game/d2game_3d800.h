// 1.13c D2Game+0x3D800: owned monster skill callback (table entry). ecx = game, edx = the unit (ret 4); the
// result is always 1. With a monster record whose 16-bit word at +0x24 is not negative, the owner flag and an
// owner (0xE03A0), the shared routine at 0x3D680 runs: eax = unit, edx = that word; game, owner, the unit's two
// values (0xB26A, 0xB0FC), the record's word +0x4C raised to at least 1, 2 and 0.
#pragma once
#include <cstdint>
#include "d2game_monrec.h"

namespace g3d800 {
template <class E> uint32_t Do(E& e, uint32_t dt, uint32_t game, uint32_t unit)
{
    const uint32_t rec = D2GameMonRec(e, dt, unit);
    if (!rec || e.rd16s(rec + 0x24) < 0 || !D2GameOwnedFlag(e, unit)) return 1;
    const uint32_t owner = D2GameOwner(e, game, unit);
    if (!owner) return 1;
    int32_t n = (int32_t)e.rd32(rec + 0x4C);
    if (n < 1) n = 1;
    const uint32_t a = e.h(0xB0FC, 0, 0, 0, 0, 0, unit);
    const uint32_t b = e.h(0xB26A, 0, 0, 0, 0, 0, unit);
    e.h(0x3D680, unit, 0, (uint32_t)(int32_t)e.rd16s(rec + 0x24), 0, 0, game, owner, b, a);
    const uint32_t w[3] = { (uint32_t)n, 2, 0 };
    e.x(0x3D680, w, 3);
    return 1;
}
}
