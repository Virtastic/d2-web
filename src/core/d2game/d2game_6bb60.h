// 1.13c D2Game+0x6BB60: object operate callback "portal to a level" (table entry). ecx = the operate block.
// The player (+8) must be a unit of type 0 (fatal line 0xFB3). The object's portal flags (0xAEE0: object) get
// bits 0 and 2 (0xAEC8: object, the byte). The target level is 0x49 for object class 0x64, the object data's
// byte +4 for class 0x179, else 0. Its act (0xA56E: level, a byte) must be below 5 (fatal line 0xFCB). With
// the player's size (0xA71E: player) a room of that level is found (0xABA4: the act's map at game +0xBC + 4 *
// act, level, 0, out x, out y, size); none: fatal line 0xFCD. A free spot near that point is searched
// (0xA694: room, the point, size, 0x1C09, 0); with one, the player is placed there (0xE19C0: eax = y, ecx = x,
// edx = the spot's room; stack: game, player, 0, 0), a failure being fatal line 0xFD9. The result is 0.
#pragma once
#include <cstdint>
#include "d2game_fatal.h"

namespace g6bb60 {
template <class E> uint32_t Operate(E& e, uint32_t op)
{
    const uint32_t kMod = 0xB2000000u;
    const uint32_t pl = e.rd32(op + 8);
    if (!pl || e.rd32(pl) != 0) { d2gfatal::Fatal(e, 0xFB3, kMod); return 0; }
    const uint32_t f = e.h(0xAEE0, 0, 0, 0, 0, 0, e.rd32(op + 4));
    e.h(0xAEC8, 0, 0, 0, 0, 0, e.rd32(op + 4), (f | 5) & 0xFF);
    const uint32_t obj = e.rd32(op + 4);
    uint32_t level = 0;
    if (obj) {
        const uint32_t cls = e.rd32(obj + 4);
        if (cls == 0x64) level = 0x49;
        else if (cls == 0x179) level = e.rd8(e.rd32(obj + 0x14) + 4);
    }
    uint32_t pt[2] = { 0, 0 };
    const uint32_t act = e.h(0xA56E, 0, 0, 0, 0, 0, level) & 0xFF;
    if (act >= 5) { d2gfatal::Fatal(e, 0xFCB, kMod); return 0; }
    const uint32_t size = e.h(0xA71E, 0, 0, 0, 0, 0, e.rd32(op + 8));
    const uint32_t room = e.h(0xABA4, 0, 0, 0, 0, 0, e.rd32(e.rd32(op) + act * 4 + 0xBC), level, 0,
                              (uint32_t)(uintptr_t)&pt[0], (uint32_t)(uintptr_t)&pt[1], size);
    if (!room) { d2gfatal::Fatal(e, 0xFCD, kMod); return 0; }
    uint32_t at[2] = { pt[0], pt[1] };
    const uint32_t size2 = e.h(0xA71E, 0, 0, 0, 0, 0, e.rd32(op + 8));
    const uint32_t spot = e.h(0xA694, 0, 0, 0, 0, 0, room, (uint32_t)(uintptr_t)at, size2, 0x1C09, 0);
    e.blk(0xA694, at, 2);
    if (!spot) return 0;
    if (!e.h(0xE19C0, at[1], at[0], spot, 0, 0, e.rd32(op), e.rd32(op + 8), 0, 0)) d2gfatal::Fatal(e, 0xFD9, kMod);
    return 0;
}
}
