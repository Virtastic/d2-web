// 1.13c D2Game+0x6DFC0: object operate callback "stair" (table entry). ecx = the operate block. The object's
// room (0xA478), its position (0xA544: object, out pair), 0xA71E (object) and 0xB13E (object) are read. By the
// object's mode (+0x10, 0 without an object):
//   mode 0: the object goes to mode 1 (0xA4A2) and, with the class record (0xA520: class), the mode timer is
//           queued (0x8EA80).
//   mode 2: the room's tile list (room +0x74, next +0xE8) is searched for a tile of kind 5 (+0); failing that
//           the same search runs over the room's neighbours (0xA55C: room, out list, out count), the room
//           itself skipped. On the first such tile the player is taken through it (0xE1C00: esi = player;
//           stack: game).
// The result is 1.
#pragma once
#include <cstdint>
#include "d2game_objop.h"

namespace g6dfc0 {
template <class E> uint32_t Kind5(E& e, uint32_t room)
{
    for (uint32_t t = e.rd32(room + 0x74); t; t = e.rd32(t + 0xE8))
        if (e.rd32(t) == 5) return t;
    return 0;
}
template <class E> uint32_t Operate(E& e, uint32_t op)
{
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, e.rd32(op + 4));
    uint32_t pos[2] = { 0, 0 };
    e.h(0xA544, 0, 0, 0, 0, 0, e.rd32(op + 4), (uint32_t)(uintptr_t)pos);
    e.h(0xA71E, 0, 0, 0, 0, 0, e.rd32(op + 4));
    e.h(0xB13E, 0, 0, 0, 0, 0, e.rd32(op + 4));
    const uint32_t obj = e.rd32(op + 4);
    const uint32_t mode = obj ? e.rd32(obj + 0x10) : 0;
    if (mode == 0) {
        e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 1);
        gobjop::ModeTimer(e, op, e.h(0xA520, 0, 0, 0, 0, 0, e.rd32(op + 0x10)));
        return 1;
    }
    if (mode != 2) return 1;
    uint32_t tile = Kind5(e, room);
    if (!tile) {
        uint32_t list = 0, count = 0;
        e.h(0xA55C, 0, 0, 0, 0, 0, room, (uint32_t)(uintptr_t)&list, (uint32_t)(uintptr_t)&count);
        for (uint32_t i = 0; i < count && !tile; i++) {
            const uint32_t r = e.rd32(list + 4 * i);
            if (r != room) tile = Kind5(e, r);
        }
        if (!tile) return 1;
    }
    e.h(0xE1C00, 0, 0, 0, 0, e.rd32(op + 8), e.rd32(op));
    return 1;
}
}
