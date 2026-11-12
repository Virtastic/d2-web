// 1.13c D2Game+0x6CD40: object operate callback "hell gate portal" (table entry). ecx = the operate block.
// The object's room (0xA478) gives its level (0xA556). Without an object or with the object in mode 0: 0.
// The player's quest flags (player data 0xA4C0, word +0x10 + 4 * the game's difficulty byte +0x6D) are tested
// (0xA568: flags, 0x16, 0xD; when clear: flags, 0x16, 0); both clear: 0. Otherwise the portal is taken
// (0x366C0: eax = game, esi = player; stack: level, object) and, on level 0x66, the waypoint record of level
// 0x67 is read (0xAB02: 0x67, out word); when there is one it is activated in the player's waypoint flags
// (player data +0x1C + 4 * difficulty; 0xAC1C: flags, the word) and the level change is sent (0x9E170: eax =
// player, edi = 0x67; stack: game, 0). The result is 0.
#pragma once
#include <cstdint>

namespace g6cd40 {
template <class E> uint32_t Operate(E& e, uint32_t op)
{
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, e.rd32(op + 4));
    const uint32_t lvl = e.h(0xA556, 0, 0, 0, 0, 0, room);
    const uint32_t obj = e.rd32(op + 4);
    if (!obj || !e.rd32(obj + 0x10)) return 0;
    uint32_t game = e.rd32(op);
    uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(op + 8));
    if (!e.h(0xA568, 0, 0, 0, 0, 0, e.rd32(pd + e.rd8(game + 0x6D) * 4 + 0x10), 0x16, 0xD)) {
        game = e.rd32(op);
        pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(op + 8));
        if (!e.h(0xA568, 0, 0, 0, 0, 0, e.rd32(pd + e.rd8(game + 0x6D) * 4 + 0x10), 0x16, 0)) return 0;
    }
    e.h(0x366C0, e.rd32(op), 0, 0, 0, e.rd32(op + 8), lvl, e.rd32(op + 4));
    if (lvl != 0x66) return 0;
    game = e.rd32(op);
    pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(op + 8));
    const uint32_t wp = e.rd32(pd + e.rd8(game + 0x6D) * 4 + 0x1C);
    uint32_t slot = op;   // the local is the saved ecx until the lookup fills it
    if (e.h(0xAB02, 0, 0, 0, 0, 0, 0x67, (uint32_t)(uintptr_t)&slot)) e.h(0xAC1C, 0, 0, 0, 0, 0, wp, slot);
    e.h(0x9E170, e.rd32(op + 8), 0, 0, 0, 0, e.rd32(op), 0);
    e.j(0x9E170, 0x67);
    return 0;
}
}
