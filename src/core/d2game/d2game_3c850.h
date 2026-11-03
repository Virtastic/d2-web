// 1.13c D2Game+0x3C850 (MISSMODE_SrvHit29_FrozenOrb), transcribed from the 1.13c disassembly. Convention: ECX = pGame, EDX = missile,
// one unused stack word (ret 4). Returns 1 (no class row / no sub-missile / no owner), 2 (0xB102 says the missile is dead) or 3.
// Row = *(*0xF829C)[0xB64] + class * 0x1A4 (count at 0xB6C); the word at row+0x24 is the hit sub-missile, row+0x4C the stride.
// 0xB102 (stdcall missile), 0xE0A20 (EAX = missile, stack game) gives the owner. The 23-dword request {2, owner, missile, ., sub-missile,
// ., ., tableB[i], tableA[i], ., ., skill (0xB26A), level (0xB0FC)} goes through 0x6F930 (ECX = game, EDX = request) once per byte offset
// i = 0, 4*max(stride,1), ... below 0x100 into the two 64-dword circle tables (A = 0xFC690, B = 0xFC790); a non-null result gets
// 0xA50E(result, B[i]) and 0xAF5E(result, A[i]).
#pragma once
#include <cstdint>

namespace r3c850 {
template <class E> uint32_t Do(E& e, uint32_t game, uint32_t missile, uint32_t dt, uint32_t tabA, uint32_t tabB)
{
    if (!missile) return 1;
    const int32_t cls = (int32_t)e.rd32(missile + 4);
    if (cls < 0 || cls >= (int32_t)e.rd32(dt + 0xB6C)) return 1;
    const uint32_t row = e.rd32(dt + 0xB64) + (uint32_t)cls * 0x1A4;
    if (!row) return 1;
    const int32_t sub = e.rd16s(row + 0x24);
    if (sub < 0) return 1;
    if (e.h(0xB102, 0, 0, 0, 0, 0, missile)) return 2;
    const uint32_t owner = e.h(0xE0A20, missile, 0, 0, 0, 0, game);
    if (!owner) return 1;
    uint32_t b[23] = {};
    b[0] = 2;
    b[1] = owner;
    b[2] = missile;
    b[11] = e.h(0xB26A, 0, 0, 0, 0, 0, missile);
    b[12] = e.h(0xB0FC, 0, 0, 0, 0, 0, missile);
    b[4] = (uint32_t)sub;
    int32_t stride = (int32_t)e.rd32(row + 0x4C);
    if (stride < 1) stride = 1;
    const int32_t step = stride << 2;
    for (int32_t i = 0; i < 0x100; i += step) {
        b[8] = e.rd32(tabA + (uint32_t)i);
        b[7] = e.rd32(tabB + (uint32_t)i);
        const uint32_t r = e.h(0x6F930, 0, game, e.stk(b));
        e.blk(0x6F930, b, 23);
        if (r) {
            e.h(0xA50E, 0, 0, 0, 0, 0, r, e.rd32(tabB + (uint32_t)i));
            e.h(0xAF5E, 0, 0, 0, 0, 0, r, e.rd32(tabA + (uint32_t)i));
        }
    }
    return 3;
}
}
