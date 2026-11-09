// 1.13c D2Game+0xED880 -- read the iron golem section of a player save. EAX = game, EBX = pEnd, ESI = player; stack (ret 8):
// ppCursor, s1. Section = 'kf' tag word, a flag byte, then (flag != 0) a golem item read through 0xED710. Returns 0 or 0x17.
// Callees: 0xA3F4 (ECX = player, EDX = 0x5A), 0xA496, 0xED710 (EAX = game; esi, &golem, &cursor, pEnd, s1), 0xA4A2, 0xA4C0, 0x49B40.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_ed880(uint32_t game, uint32_t pEnd, uint32_t player, uint32_t ppCursor, uint32_t s1, const D2Seam* e)
{
    const uint32_t cur = d2rd32(ppCursor);
    if (!d2rd32(game + 0x70) || cur + 2 > pEnd) return 0;
    if (d2rd16(cur) != 0x666b) return 0x17;
    uint32_t local = cur + 3, golem = 0;
    if (d2rd8(cur + 2)) {
        const uint32_t r = e->call(0xA3F4, 0, player, 0x5a);
        if (!r) return 0x17;
        e->call(0xA496, 0, 0, 0, 0, 0, player, r);
        if (!e->call(0xED710, game, 0, 0, 0, 0, player, (uint32_t)(uintptr_t)&golem, (uint32_t)(uintptr_t)&local, pEnd, s1)) return 0x17;
        e->call(0xA4A2, 0, 0, 0, 0, 0, golem, 3);
        const uint32_t id = golem ? d2rd32(golem + 0xc) : 0xffffffffu;
        uint32_t cl = 0;
        if (player && d2rd32(player) == 0) cl = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, player) + 0x9c);
        e->call(0x49B40, cl, 0, 0, 0, 0, id);
    }
    d2wr32(ppCursor, local);
    return 0;
}
