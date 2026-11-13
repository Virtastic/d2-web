// 1.13c D2Game+0x3C3B0: missile hit callback (table entry) that throws a ring of sub-missiles. ecx = game,
// edx = the missile, one stack word (unread; ret 4). Always 1. Nothing happens without a missile record, with
// a negative sub-missile class (record short +0x24) or with a count (record +0x4C) below 1. Otherwise a zeroed
// 23-word request is filled: [0] = 3, [1] = the missile's owner (0xE0A20: eax = missile, game), [4] = the
// sub-missile class, [5]/[6] = the missile's position (0x1100 / 0x1130, ecx = missile), [11] = its skill
// (0xB26A), [12] = its skill level (0xB0FC), [21] = the routine at 0x39210 when record +0x50 is nonzero; and
// 0xA26E0 (esi = the request; game, count) throws the ring.
#pragma once
#include <cstdint>
#include "d2game_misshit.h"

namespace g3c3b0 {
template <class E> uint32_t Hit(E& e, uint32_t dt, uint32_t game, uint32_t unit)
{
    const uint32_t rec = D2GameMissRec(e, dt, unit);
    if (!rec || e.rd16s(rec + 0x24) < 0 || (int32_t)e.rd32(rec + 0x4C) <= 0) return 1;
    uint32_t b[23] = {};
    b[0] = 3;
    b[5] = e.h(0x1100, 0, unit);
    b[6] = e.h(0x1130, 0, unit);
    b[11] = e.h(0xB26A, 0, 0, 0, 0, 0, unit);
    b[1] = e.h(0xE0A20, unit, 0, 0, 0, 0, game);
    b[4] = (uint32_t)e.rd16s(rec + 0x24);
    b[12] = e.h(0xB0FC, 0, 0, 0, 0, 0, unit);
    if (e.rd32(rec + 0x50)) b[21] = 0xB2000000u | 0x39210;
    e.h(0xA26E0, 0, 0, 0, 0, e.stk(b), game, e.rd32(rec + 0x4C));
    e.blk(0xA26E0, b, 23);
    return 1;
}
}
