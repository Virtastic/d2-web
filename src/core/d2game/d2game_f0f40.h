// 1.13c D2Game+0xF0F40: player relation callback with a notice (table entry). ecx = the game, edx = player A,
// three stack words (ret 0xC): player B, a flag, one unread. A's client record is read (0xA4C0: A, record at
// +0x9C of the player data) when A is a unit of type 0; bit 2 of its byte +0x0A must be set, else the record
// address is returned as is. The relation is applied (0xF0C60: eax = B, ecx = A, edx = the game; stack: 1, the
// flag). The notice code is 2 or 0xB (flag set or clear) when A's class (+0x04) is 0, 1 or 6, otherwise 1 or 0xA.
// It is sent to B's client record (0xF0720: edx = A's unit id at +0x0C, or -1 without A; stack: B's record, or
// null when B is not a type 0 unit, and the code), whose result is returned.
#pragma once
#include <cstdint>

namespace gf0f40 {
template <class E> uint32_t Notice(E& e, uint32_t game, uint32_t a, uint32_t b, uint32_t flag)
{
    uint32_t rec = 0;
    if (a && e.rd32(a) == 0) rec = e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, a) + 0x9C);
    if (!(e.rd8(rec + 0xA) & 4)) return rec;
    e.h(0xF0C60, b, a, game, 0, 0, 1, flag);
    uint32_t code;
    if (a && (e.rd32(a + 4) <= 1 || e.rd32(a + 4) == 6)) code = flag ? 2 : 0xB;
    else code = flag ? 1 : 0xA;
    const uint32_t id = a ? e.rd32(a + 0xC) : 0xFFFFFFFFu;
    uint32_t brec = 0;
    if (b && e.rd32(b) == 0) brec = e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, b) + 0x9C);
    return e.h(0xF0720, 0, 0, id, 0, 0, brec, code);
}
}
