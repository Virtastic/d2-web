// 1.13c D2Game+0x5E2A0: object operate callback "summit portal door" (table entry). ecx = the operate block.
// The quest record 0x23 is looked up; none: 0. The player's quest flags (player data 0xA4C0, word +0x10 + 4 *
// the game's difficulty byte +0x6D) are tested for the record's quest (+0xE0) with bits 0 then 1 (0xA568).
// The door stays shut, with the sound 0x13 on the player (0xDFFD0: esi = player; stack 0x13, player), when
// both are clear, when the record flag +9 is set and data byte +0 is clear, or when data byte +0x48 is set
// and data byte +0 is clear. Otherwise, with the class record (0xA520: class) and the object's mode (+0x10, 0
// without an object):
//   mode 0: the object goes to mode 1 (0xA4A2), its room (0xA478) is refreshed (0xA51A: room, 0) and the mode
//           timer is queued (0x8EA80: ecx = game, edx = object; 1, frame + (record +0xDC >> 8), 0, 0, 0).
//   mode 2: the player is warped (0xC8950: ebx = block) and the room refreshed (0xA51A: room, 1).
// The result is 0.
#pragma once
#include <cstdint>
#include "d2game_objinit.h"

namespace g5e2a0 {
template <class E> uint32_t Operate(E& e, uint32_t op)
{
    const uint32_t game = e.rd32(op);
    const uint32_t q = gobjinit::Find(e, op, 0x23);
    if (!q) return 0;
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(op + 8));
    const uint32_t flags = e.rd32(pd + e.rd8(game + 0x6D) * 4 + 0x10);
    bool open = e.h(0xA568, 0, 0, 0, 0, 0, flags, e.rd32(q + 0xE0), 0) || e.h(0xA568, 0, 0, 0, 0, 0, flags, e.rd32(q + 0xE0), 1);
    if (open) {
        const uint32_t d = e.rd32(q + 0x18);
        if (e.rd8(q + 9) && !e.rd8(d)) open = false;
        else if (e.rd8(d + 0x48) && !e.rd8(d)) open = false;
    }
    if (!open) {
        const uint32_t pl = e.rd32(op + 8);
        e.h(0xDFFD0, 0, 0, 0, 0, pl, 0x13, pl);
        return 0;
    }
    const uint32_t rec = e.h(0xA520, 0, 0, 0, 0, 0, e.rd32(op + 0x10));
    const uint32_t obj = e.rd32(op + 4);
    const uint32_t mode = obj ? e.rd32(obj + 0x10) : 0;
    if (mode == 0) {
        e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 1);
        e.h(0xA51A, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, e.rd32(op + 4)), 0);
        const uint32_t g = e.rd32(op);
        e.h(0x8EA80, 0, g, e.rd32(op + 4), 0, 0, 1, (e.rd32(rec + 0xDC) >> 8) + e.rd32(g + 0xA8), 0, 0, 0);
    } else if (mode == 2) {
        e.h(0xC8950, 0, 0, 0, op);
        e.h(0xA51A, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, e.rd32(op + 4)), 1);
    }
    return 0;
}
}
