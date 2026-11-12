// 1.13c D2Game+0x63000 (LAB_6fc83000): the quest 6 player-join callback (ECX = game, EDX = player, one unused stack word, ret 4).
// The player's quest flags for the game's difficulty (#A4C0(player) +0x10 + 4 * game byte +0x6D) decide it: nothing happens when
// #A568(flags, 6, 0) or #A568(flags, 6, 1) is 1, when the player has no #A478 / #A556 chain, or when #A56E of it is set (low byte).
// Otherwise the game's quest list (+0x10F4 -> first record, linked through +0xF4) gives the record with id 6; its data (+0x18) takes
// the player's id (+0x0C, or -1 with no player) into three GUID lists (+0x00, +0x84, +0x108) through 0x33880 (EAX = list, ESI = id),
// then 0x62640 (EAX = flags, ECX = player; game) runs. Retail returns nothing; the EAX it leaves (the last callee's return) is returned.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_63000(uint32_t game, uint32_t unit, const D2Seam* e)
{
    const uint32_t pd = e->call(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t flags = d2rd32(pd + 0x10 + 4 * (uint32_t)d2rd8(game + 0x6D));
    uint32_t v = e->call(0xA568, 0, 0, 0, 0, 0, flags, 6, 0);
    if (v == 1) return v;
    v = e->call(0xA568, 0, 0, 0, 0, 0, flags, 6, 1);
    if (v == 1) return v;
    v = e->call(0xA478, 0, 0, 0, 0, 0, unit);
    if (!v) return v;
    v = e->call(0xA556, 0, 0, 0, 0, 0, v);
    if (!v) return v;
    v = e->call(0xA56E, 0, 0, 0, 0, 0, v);
    if (v & 0xFF) return v;
    uint32_t q = d2rd32(game + 0x10F4);
    if (q) {
        q = d2rd32(q);
        while (q && d2rd32(q) != 6) q = d2rd32(q + 0xF4);
    }
    const uint32_t data = d2rd32(q + 0x18);
    const uint32_t pid = unit ? d2rd32(unit + 0xC) : 0xFFFFFFFFu;
    e->call(0x33880, data, 0, 0, 0, pid);
    e->call(0x33880, data + 0x84, 0, 0, 0, pid);
    e->call(0x33880, data + 0x108, 0, 0, 0, pid);
    return e->call(0x62640, flags, unit, 0, 0, 0, game);
}
