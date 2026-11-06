// 1.13c D2Game+0x5A290 (ACT5Q2_ResolveSoldierUnit): EDI = pGame, stack (unit, out), ret 8. Walk the quest list at game+0x10F4 for the node of type 0x20
// (nodes chained at +0xF4); the slot index is 0x58260(EAX = unit, ECX = node) (the unit is the stack argument, not the node). A negative index or a clear
// present byte (node+0x18 record +0xF8+i) returns 0; otherwise the stored id (record +0xEC+4*i) is looked up in the game's unit hash (game+0x1520, 128
// buckets by id & 0x7F, chained at +0xE4, id at +0xC); a hit stores the unit at *out and returns 1.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_5a290(uint32_t game, uint32_t unit, uint32_t out, const D2Seam* e)
{
    const uint32_t list = d2rd32(game + 0x10F4);
    if (!list) return 0;
    uint32_t n = d2rd32(list);
    while (n && d2rd32(n) != 0x20) n = d2rd32(n + 0xF4);
    if (!n) return 0;
    const uint32_t rec = d2rd32(n + 0x18);
    const int32_t i = (int32_t)e->call(0x58260, unit, n);
    if (i < 0) return 0;
    if (!d2rd8(rec + 0xF8 + i)) return 0;
    const uint32_t id = d2rd32(rec + 0xEC + 4 * (uint32_t)i);
    for (uint32_t p = d2rd32(game + 0x1520 + (id & 0x7F) * 4); p; p = d2rd32(p + 0xE4))
        if (d2rd32(p + 0xC) == id) { d2wr32(out, p); return 1; }
    return 0;
}
