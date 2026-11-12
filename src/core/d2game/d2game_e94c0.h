// 1.13c D2Game+0xE94C0: object operate callback "tome stand" (table entry). ecx = the operate block. The
// quest record 0xF is looked up; none, its flag +9 clear, or an object past its neutral mode: 0. A player
// whose quest flags (player data 0xA4C0, word +0x10 + 4 * the game's difficulty byte +0x6D) have quest 0x11
// bit 0 (0xA568 answers 1) only hears the sound 0x13 (0xDFFD0: esi = player; stack 0x13, player). Otherwise
// the object's drop code (+0xB8) becomes "bbb " and the book is made (0x11A70: ebx = game; stack: object, 2, a
// local, 0, -1, 0); none: 0. The object goes to mode 2 (0xA4A2); the quest data takes byte +2 = 1, a count at
// +8, byte +1 = 1, word +0x18 = 2 and the object's id at +0x14 (-1 without one); the record's handlers at
// +0xA0 and +0xCC become 0xE5270 and 0xE8FE0; a state (+0x0C) other than 3 is moved to 3 (0x35EB0: eax 3, esi
// = record); a byte +0x0B other than 1 becomes 1 with byte +0x14 cleared. The result is 0.
#pragma once
#include <cstdint>
#include "d2game_objinit.h"

namespace ge94c0 {
template <class E> uint32_t Operate(E& e, uint32_t op)
{
    const uint32_t game = e.rd32(op);
    const uint32_t q = gobjinit::Find(e, op, 0xF);
    if (!q || !e.rd8(q + 9)) return 0;
    const uint32_t obj = e.rd32(op + 4);
    if (obj && e.rd32(obj + 0x10)) return 0;
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(op + 8));
    if (e.h(0xA568, 0, 0, 0, 0, 0, e.rd32(pd + e.rd8(game + 0x6D) * 4 + 0x10), 0x11, 0) == 1) {
        const uint32_t pl = e.rd32(op + 8);
        e.h(0xDFFD0, 0, 0, 0, 0, pl, 0x13, pl);
        return 0;
    }
    uint32_t local = op;   // the callee's out word; nothing reads it back
    e.wr32(e.rd32(op + 4) + 0xB8, 0x20626262);
    if (!e.h(0x11A70, 0, 0, 0, e.rd32(op), 0, e.rd32(op + 4), 2, (uint32_t)(uintptr_t)&local, 0, 0xFFFFFFFFu, 0)) return 0;
    e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 2);
    const uint32_t d = e.rd32(q + 0x18);
    e.wr8(d + 2, 1);
    e.wr32(d + 8, e.rd32(d + 8) + 1);
    e.wr8(d + 1, 1);
    e.wr32(d + 0x18, 2);
    e.wr32(d + 0x14, gobjinit::ObjId(e, e.rd32(op + 4)));
    e.wr32(q + 0xA0, 0xB20E5270u);
    e.wr32(q + 0xCC, 0xB20E8FE0u);
    if (e.rd8(q + 0xC) != 3) e.h(0x35EB0, 3, 0, 0, 0, q);
    if (e.rd8(q + 0xB) != 1) { e.wr8(q + 0x14, 0); e.wr8(q + 0xB, 1); }
    return 0;
}
}
