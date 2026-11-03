// 1.13c D2Game+0x40CA0 (MISSMODE_SrvDo15_FrozenOrb), transcribed from the 1.13c disassembly. Convention: ECX = pGame, EDX = missile, plain
// ret. Returns 2 for a missing missile / class row / sub-missile (row+0x18 < 0) / owner (flag bit 10 of missile+0xC8 clear, or
// 0xE03A0 (EAX = owner type at +0x94, ECX = game, EDX = guid at +0x98) null), else 0x3F320(game) (stdcall).
// The frame counter 0xB28E(missile) is taken modulo max(row+0x38, 1); a positive remainder only runs 0x3F320. Otherwise the 23-dword
// request {2, owner, missile, ., sub-missile (row+0x18), ., ., tableA[d], tableB[d], ., ., skill (0xB26A), level (0xB0FC)} (tableA at
// 0xFCA60, tableB at 0xFC960) is built with d = 0xB24C(missile) folded into 0..63; 0xA50E(missile, (row+0x3C + d) folded) stores the
// next direction; 0x6F930 (ECX = game, EDX = request) creates the sub-missile, whose result is ignored.
#pragma once
#include <cstdint>

namespace r40ca0 {
static inline uint32_t fold(uint32_t v, bool negate)
{
    v &= 0x8000003Fu;
    if ((int32_t)v < 0) {
        v = ((v - 1) | 0xFFFFFFC0u) + 1;
        if (negate && (int32_t)v < 0) v = 0u - v;
    }
    return v;
}
template <class E> uint32_t Do(E& e, uint32_t game, uint32_t missile, uint32_t dt, uint32_t tabA, uint32_t tabB)
{
    if (!missile) return 2;
    const int32_t cls = (int32_t)e.rd32(missile + 4);
    if (cls < 0 || cls >= (int32_t)e.rd32(dt + 0xB6C)) return 2;
    const uint32_t row = e.rd32(dt + 0xB64) + (uint32_t)cls * 0x1A4;
    if (!row) return 2;
    const int32_t sub = e.rd16s(row + 0x18);
    if (sub < 0) return 2;
    if (!((e.rd32(missile + 0xC8) >> 10) & 1)) return 2;
    const uint32_t owner = e.h(0xE03A0, e.rd32(missile + 0x94), game, e.rd32(missile + 0x98));
    if (!owner) return 2;
    int32_t div = (int32_t)e.rd32(row + 0x38);
    if (div < 1) div = 1;
    const int32_t fr = (int32_t)e.h(0xB28E, 0, 0, 0, 0, 0, missile);
    if (fr % div > 0) return e.h(0x3F320, 0, 0, 0, 0, 0, game);
    uint32_t b[23] = {};
    b[0] = 2;
    b[1] = owner;
    b[2] = missile;
    b[11] = e.h(0xB26A, 0, 0, 0, 0, 0, missile);
    b[12] = e.h(0xB0FC, 0, 0, 0, 0, 0, missile);
    b[4] = (uint32_t)sub;
    const uint32_t d = fold(e.h(0xB24C, 0, 0, 0, 0, 0, missile), true);
    b[7] = e.rd32(tabA + 4 * d);
    b[8] = e.rd32(tabB + 4 * d);
    const uint32_t nd = fold(e.rd32(row + 0x3C) + d, false);
    e.h(0xA50E, 0, 0, 0, 0, 0, missile, nd);
    e.h(0x6F930, 0, game, e.stk(b));
    e.blk(0x6F930, b, 23);
    return e.h(0x3F320, 0, 0, 0, 0, 0, game);
}
}
