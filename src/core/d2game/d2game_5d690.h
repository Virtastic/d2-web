// 1.13c D2Game+0x5D690: object operate callback "summit door" (table entry). ecx = the operate block. The
// quest record 0x23 is looked up in the game's list; none: 0. With its data (record +0x18) and the object's
// mode (+0x10, 0 without an object):
//   mode 0: data byte +0 clear: the object goes to mode 1 (0xA4A2) and data +0x60 = 2; else the player is
//           warped (0xC8950: ebx = the block).
//   mode 2: data byte +0 clear: the sound 0x13 is attached to the player (0xDFFD0: esi = player; stack 0x13,
//           player); else, with data byte +0x48 set, the object goes to mode 0 and data +0x60 = 0. Then as mode 1.
//   mode 1: data byte +0 clear: sound 0x13 on the player and the object to mode 2; else data +0x60 = 0 and
//           the object to mode 0.
// The result is 0.
#pragma once
#include <cstdint>
#include "d2game_objinit.h"

namespace g5d690 {
template <class E> uint32_t Operate(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 0x23);
    if (!q) return 0;
    const uint32_t obj = e.rd32(op + 4), data = e.rd32(q + 0x18);
    const uint32_t mode = obj ? e.rd32(obj + 0x10) : 0;
    if (mode == 0) {
        if (!e.rd8(data)) {
            e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 1);
            e.wr32(data + 0x60, 2);
        } else {
            e.h(0xC8950, 0, 0, 0, op);
        }
        return 0;
    }
    if (mode != 1 && mode != 2) return 0;
    if (mode == 2) {
        if (!e.rd8(data)) {
            const uint32_t pl = e.rd32(op + 8);
            e.h(0xDFFD0, 0, 0, 0, 0, pl, 0x13, pl);
        } else if (e.rd8(data + 0x48)) {
            e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 0);
            e.wr32(data + 0x60, 0);
        }
    }
    if (!e.rd8(data)) {
        const uint32_t pl = e.rd32(op + 8);
        e.h(0xDFFD0, 0, 0, 0, 0, pl, 0x13, pl);
        e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 2);
    } else {
        e.wr32(data + 0x60, 0);
        e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 0);
    }
    return 0;
}
}
