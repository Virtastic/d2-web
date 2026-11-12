// 1.13c D2Game+0xEC260: 'JM' corpse section, stdcall (game, player, ppCursor, pEnd, a4, a5), ret 0x18.
// The player's inventory (+0x60) is required (none -> 2). Every corpse of it (0xA466 first / 0xA484 next, id from 0xA4AE) is
// looked up in the game's unit hash (game+0x1120 + (id & 0x7F)*4, chained through +0xE4, match on +0xC; a null game is the fatal
// path, line 0x2D3). A found unit with an inventory holding items (0xA490 > 0) is walked (0xA3DC / 0xA4CC, unit 0xA46C): an item
// without flag 4 (0xA430) goes through the quest write 0xA406(player, item, 3, playerData[+0x10 + difficulty*4], 0x201, 0),
// which adds to a running count that starts at 1. The corpse unit with the largest count (a later one wins a tie) is the one
// written. Then: cursor + 4 > pEnd -> 1; 'JM' and a word count (1 when a unit was chosen, else 0); with no unit the cursor ends
// there. Else 12 more bytes (cursor + 12 > pEnd -> 1): a dword the code never sets (stack residue; written as 0), then 0x1100(unit)
// and 0x1130(unit); then 0xEC0E0 (EAX = pEnd, ECX = a5, EDX = a4, EDI = &cursor, stack game, unit, 0) appends the unit's items
// (nonzero -> 4); the cursor it leaves is stored.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_ec260(uint32_t game, uint32_t player, uint32_t ppCursor, uint32_t pEnd, uint32_t a4, uint32_t a5, const D2Seam* e)
{
    uint32_t out = d2rd32(ppCursor);
    const uint32_t inv = d2rd32(player + 0x60);
    if (!inv) return 2;
    uint32_t best = 0, bestUnit = 0;
    uint32_t it = e->call(0xA466, 0, 0, 0, 0, 0, inv);
    while (it) {
        const uint32_t id = e->call(0xA4AE, 0, 0, 0, 0, 0, it);
        if (!game) {                                              // D2_ASSERT, line 0x2D3
            const uint32_t t = e->call(0xA2DA, 0, 0, 0, 0, 0, 0x2D3);
            e->call(0xA2C2, 0, 0, 0, 0, 0, 0x6FD19FF9u, t);
            e->call(0x2A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
        }
        uint32_t u = d2rd32(game + 0x1120 + (id & 0x7F) * 4);
        while (u && d2rd32(u + 0xC) != id) u = d2rd32(u + 0xE4);
        if (u) {
            const uint32_t uinv = d2rd32(u + 0x60);
            if (uinv && e->call(0xA490, 0, 0, 0, 0, 0, uinv) != 0) {
                uint32_t n = 1;
                for (uint32_t b = e->call(0xA3DC, 0, 0, 0, 0, 0, uinv); b; b = e->call(0xA4CC, 0, 0, 0, 0, 0, b)) {
                    const uint32_t item = e->call(0xA46C, 0, 0, 0, 0, 0, b);
                    if (e->call(0xA430, 0, 0, 0, 0, 0, item, 4) == 0) {
                        const uint32_t pd = e->call(0xA4C0, 0, 0, 0, 0, 0, player);
                        const uint32_t rec = d2rd32(pd + d2rd8(game + 0x6D) * 4 + 0x10);
                        n += e->call(0xA406, 0, 0, 0, 0, 0, player, item, 3, rec, 0x201, 0);
                    }
                }
                if (n >= best) { best = n; bestUnit = u; }
            }
        }
        it = e->call(0xA484, 0, 0, 0, 0, 0, it);
    }
    if (out + 4 > pEnd) return 1;
    const uint32_t cnt = bestUnit ? 1 : 0;
    d2wr16(out, 0x4D4A);                                          // 'JM'
    d2wr16(out + 2, cnt);
    out += 4;
    if (cnt) {
        if (out + 12 > pEnd) return 1;
        const uint32_t x = e->call(0x1100, 0, bestUnit);
        const uint32_t y = e->call(0x1130, 0, bestUnit);
        d2wr32(out, 0);
        d2wr32(out + 4, x);
        d2wr32(out + 8, y);
        uint32_t slot = out + 12;
        const uint32_t rc = e->call(0xEC0E0, pEnd, a5, a4, 0, 0, game, bestUnit, 0);
        e->edi(0xEC0E0, (uint32_t)(uintptr_t)&slot);
        if (rc) return 4;
        out = slot;
    }
    d2wr32(ppCursor, out);
    return 0;
}
