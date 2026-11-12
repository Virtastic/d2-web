// 1.13c D2Game+0x6B2E0: object operate callback "armor stand" (table entry). ecx = the operate block ([0] game,
// [4] the object). An object already past its neutral mode (+0x10 non-zero): 1, nothing done. Otherwise the
// object's position is read (0xA544: object, out pair), its room found (0xA478: object) and the drop made
// (0x11FB0: eax = the position pair, ecx = room, ebx = game; stack: -1, 0, 0); the object goes to mode 2
// (0xA4A2: object, 2) and, when there is an object, bit 1 of its flag word (+0xC4) is cleared. The result is 1.
#pragma once
#include <cstdint>

namespace g6b2e0 {
template <class E> uint32_t Operate(E& e, uint32_t op)
{
    uint32_t obj = e.rd32(op + 4);
    if (obj && e.rd32(obj + 0x10)) return 1;
    uint32_t pos[2];
    const uint32_t pp = (uint32_t)(uintptr_t)pos;
    e.h(0xA544, 0, 0, 0, 0, 0, obj, pp);
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, e.rd32(op + 4));
    e.h(0x11FB0, pp, room, 0, e.rd32(op), 0, 0xFFFFFFFFu, 0, 0);
    e.blk(0x11FB0, pp, 2);
    e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 2);
    obj = e.rd32(op + 4);
    if (obj) e.wr32(obj + 0xC4, e.rd32(obj + 0xC4) & ~2u);
    return 1;
}
}
