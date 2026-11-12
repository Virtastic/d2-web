// 1.13c D2Game+0x65D40: object operate callback "malus stand" (table entry). ecx = the operate block. The
// quest record 3 is looked up; none: 0. With its flag +9 clear the object goes to mode 2 (0xA4A2) and the
// player hears the sound 0x13 (0xDFFD0: esi = player; stack 0x13, player). Otherwise an object past its
// neutral mode: 0; a player whose quest flags (player data 0xA4C0, word +0x10 + 4 * the game's difficulty byte
// +0x6D) have quest 3 bit 0 or bit 1 (0xA568 answers 1): 0; a player whose stat 0xC (0xA3E2: player, 0xC, 0)
// is below 8 only hears the sound. Then the object's drop code (+0xB8) becomes "hdm " and the item is made
// (0x11A70: ebx = game; stack: object, 2, a local, 0, -1, 0); none: 0. The object goes to mode 2; the quest
// data takes byte +1 = 1, word +0x98 = 2, the object's id at +4 (-1 without one) and a count at +0x9C; a state
// (+0x0C) other than 4 is moved to 4 (0x35EB0: eax 4, esi = record) and the players of the record's game (+4)
// are visited (0xDFCC0: eax 0, ebx = callback 0x61DA0, edi = that game; stack 0); a byte +0x0B other than 1
// becomes 1 with byte +0x14 cleared. The result is 0.
#pragma once
#include <cstdint>
#include "d2game_objinit.h"

namespace g65d40 {
template <class E> uint32_t Operate(E& e, uint32_t op)
{
    const uint32_t game = e.rd32(op);
    const uint32_t q = gobjinit::Find(e, op, 3);
    if (!q) return 0;
    const uint32_t obj = e.rd32(op + 4);
    const uint32_t mode = obj ? e.rd32(obj + 0x10) : 0;
    const auto sound = [&] { const uint32_t pl = e.rd32(op + 8); e.h(0xDFFD0, 0, 0, 0, 0, pl, 0x13, pl); };
    if (!e.rd8(q + 9)) {
        e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 2);
        sound();
        return 0;
    }
    if (mode) return 0;
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(op + 8));
    const uint32_t flags = e.rd32(pd + e.rd8(game + 0x6D) * 4 + 0x10);
    if (e.h(0xA568, 0, 0, 0, 0, 0, flags, 3, 0) == 1) return 0;
    if (e.h(0xA568, 0, 0, 0, 0, 0, flags, 3, 1) == 1) return 0;
    if ((int32_t)e.h(0xA3E2, 0, 0, 0, 0, 0, e.rd32(op + 8), 0xC, 0) < 8) { sound(); return 0; }
    uint32_t local = op;   // the callee's out word; nothing reads it back
    e.wr32(e.rd32(op + 4) + 0xB8, 0x206D6468);
    if (!e.h(0x11A70, 0, 0, 0, e.rd32(op), 0, e.rd32(op + 4), 2, (uint32_t)(uintptr_t)&local, 0, 0xFFFFFFFFu, 0)) return 0;
    e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 2);
    const uint32_t d = e.rd32(q + 0x18);
    e.wr8(d + 1, 1);
    e.wr32(d + 0x98, 2);
    e.wr32(d + 4, gobjinit::ObjId(e, e.rd32(op + 4)));
    e.wr32(d + 0x9C, e.rd32(d + 0x9C) + 1);
    if (e.rd8(q + 0xC) != 4) {
        e.h(0x35EB0, 4, 0, 0, 0, q);
        e.h(0xDFCC0, 0, 0, 0, 0xB2061DA0u, 0, 0);
        e.j(0xDFCC0, e.rd32(q + 4));
    }
    if (e.rd8(q + 0xB) != 1) { e.wr8(q + 0x14, 0); e.wr8(q + 0xB, 1); }
    return 0;
}
}
