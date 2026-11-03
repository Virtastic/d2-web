// D2Game+0x9C4F0 -- adds a player to the game's party list: EAX = game, stack player unit (ret 4). Written from the 1.13c disassembly; callees are the D2Seam.
// list = 0x9BD00(EAX = game) (null is fatal 0xFE); id = player ? player+0xC : -1; a null player or one whose unit type (+0) is not 0 is fatal 0xFF. If 0x9BCB0(EAX = list, ECX = id)
// finds the id already in the list, return 1. Otherwise a node (8 bytes, 0xA2E6 on pool game+0x1C, line 0x108; null is fatal 0x109) {id, next = list+4} becomes the new head,
// 0x8CFB0 (EAX = player, stack game) runs, and for each following member id the game hash chain game+0x1120+(id&0x7F)*4 (next at +0xE4, id at +0xC) yields the unit: 0xF4210 (EAX = unit, EDI = player),
// then 0xF4210 (EAX = player, EDI = unit). Always returns 1. The fatal paths (0xA2DA, 0xA2C2, 0x2A3C(-1)) exit the process.
#include "d2game_seam4e.h"
// 1.13c D2Game+0x9C4F0
extern "C" uint32_t d2game_9c4f0(uint32_t game, uint32_t player, const D2Seam* e)
{
    auto fatal = [&](uint32_t code) {
        const uint32_t s = e->call(0xA2DA, 0, 0, 0, 0, 0, code);
        e->call(0xA2C2, 0, 0, 0, 0, 0, 0, s);
        e->call(0x2A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
    };
    const uint32_t list = e->call(0x9BD00, game);
    const uint32_t id = player ? d2rd32(player + 0xC) : 0xFFFFFFFFu;
    if (!list) { fatal(0xFE); return 1; }
    if (!player || d2rd32(player)) { fatal(0xFF); return 1; }
    if (e->call(0x9BCB0, list, id)) return 1;
    const uint32_t node = e->call(0xA2E6, 0, d2rd32(game + 0x1C), 8, 0, 0, 0xB2000000u | 0xFB9CC, 0x108, 0);
    if (!node) { fatal(0x109); return 1; }
    d2wr32(node, id);
    d2wr32(node + 4, d2rd32(list + 4));
    d2wr32(list + 4, node);
    e->call(0x8CFB0, player, 0, 0, 0, 0, game);
    for (uint32_t m = d2rd32(node + 4); m; m = d2rd32(m + 4)) {
        const uint32_t mid = d2rd32(m);
        uint32_t u = d2rd32(game + (mid & 0x7F) * 4 + 0x1120);
        while (u && d2rd32(u + 0xC) != mid) u = d2rd32(u + 0xE4);
        if (!u) continue;
        e->call(0xF4210, u, 0, 0, 0, player);
        e->call(0xF4210, player, 0, 0, 0, u);
    }
    return 1;
}
