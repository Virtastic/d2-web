// 1.13c D2Game+0x33330: object init callback "portal" (table entry). ecx = the init block ([0] game, [4] the
// object, [8] the room, [0x10] the object's table row, [0x14] x, [0x18] y). The room's level (0xA556: room) gives
// its act byte (0xA56E: level); 0xAADE takes one word whose low byte is that act and whose upper bytes are those of
// the block's address, and its low byte goes to the object data's (+0x14) byte +0x04. An object not in mode 1
// (+0x10) answers its mode. Otherwise 0xA69A (object, room, x, y) and an event is queued (0x8EA80: ecx = game,
// edx = object; stack: 1, the game's frame (+0xA8) plus the row's word +0xDC shifted right 8 plus 1, 0, 0, 0),
// whose answer is the result.
#pragma once
#include "d2game_objinit.h"

namespace g33330 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t lvl = e.h(0xA556, 0, 0, 0, 0, 0, e.rd32(op + 8));
    const uint32_t act = e.h(0xA56E, 0, 0, 0, 0, 0, lvl);
    const uint32_t r = e.h(0xAADE, 0, 0, 0, 0, 0, (op & 0xFFFFFF00u) | (act & 0xFF));
    const uint32_t obj = e.rd32(op + 4);
    e.wr8(e.rd32(obj + 0x14) + 4, (uint8_t)r);
    const uint32_t mode = e.rd32(obj + 0x10);
    if (mode != 1) return mode;
    e.h(0xA69A, 0, 0, 0, 0, 0, obj, e.rd32(op + 8), e.rd32(op + 0x14), e.rd32(op + 0x18));
    const uint32_t game = e.rd32(op);
    const uint32_t when = (e.rd32(e.rd32(op + 0x10) + 0xDC) >> 8) + e.rd32(game + 0xA8) + 1;
    return e.h(0x8EA80, 0, game, obj, 0, 0, 1, when, 0, 0, 0);
}
}
