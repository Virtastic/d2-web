// 1.13c D2Game+0x39DA0: monster skill callback (table entry). ecx = game, edx = the unit, one stack word
// (ret 4); the result is always 1. With a monster record, both unit values (0xB26A, 0xB0FC) above zero and the
// test at 0xE0A20 (eax = unit; game) passing, a nonzero stack word is first stored on the unit (0xACD0: unit,
// word), then 0x9E420 runs (eax = the record's word +0x4C, ecx = second value, edx = first; game, unit).
#pragma once
#include <cstdint>
#include "d2game_monrec.h"

namespace g39da0 {
template <class E> uint32_t Do(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t s0)
{
    const uint32_t rec = D2GameMonRec(e, dt, unit);
    if (!rec) return 1;
    const uint32_t a = e.h(0xB26A, 0, 0, 0, 0, 0, unit);
    const uint32_t b = e.h(0xB0FC, 0, 0, 0, 0, 0, unit);
    if ((int32_t)a <= 0 || (int32_t)b <= 0) return 1;
    if (!e.h(0xE0A20, unit, 0, 0, 0, 0, game)) return 1;
    if (s0) e.h(0xACD0, 0, 0, 0, 0, 0, unit, s0);
    e.h(0x9E420, e.rd32(rec + 0x4C), b, a, 0, 0, game, unit);
    return 1;
}
}
