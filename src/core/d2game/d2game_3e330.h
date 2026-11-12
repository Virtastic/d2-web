// 1.13c D2Game+0x3E330: monster skill callback (table entry). ecx = game, edx = the unit, one stack word (ret 4);
// the result is always 3. With a monster record whose 16-bit word at +0x24 is not negative, the shared routine
// at 0x3D680 runs for the stack word's unit, or this unit when it is zero (eax), edx = that word; game, the owner
// (0xE03A0, only with the owner flag, else 0), the unit's two values (0xB26A, 0xB0FC) and the record's three
// words at +0x4C, +0x50 and +0x54.
#pragma once
#include <cstdint>
#include "d2game_monrec.h"

namespace g3e330 {
template <class E> uint32_t Do(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t s0)
{
    const uint32_t rec = D2GameMonRec(e, dt, unit);
    if (!rec || e.rd16s(rec + 0x24) < 0) return 3;
    const uint32_t who = s0 ? s0 : unit;
    const uint32_t owner = D2GameOwnedFlag(e, unit) ? D2GameOwner(e, game, unit) : 0;
    const uint32_t a = e.h(0xB0FC, 0, 0, 0, 0, 0, unit);
    const uint32_t b = e.h(0xB26A, 0, 0, 0, 0, 0, unit);
    e.h(0x3D680, who, 0, (uint32_t)(int32_t)e.rd16s(rec + 0x24), 0, 0, game, owner, b, a);
    const uint32_t w[3] = { e.rd32(rec + 0x4C), e.rd32(rec + 0x50), e.rd32(rec + 0x54) };
    e.x(0x3D680, w, 3);
    return 3;
}
}
