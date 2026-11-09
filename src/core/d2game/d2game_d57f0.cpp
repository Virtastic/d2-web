// D2Game+0xD57F0 -- equip a stored item onto a body slot. EAX = player, stack a0 game, a1 item id, a2 slot (ret 0xC).
// Written from the 1.13c disassembly. game == 0 or player == 0 and the 0xA748 / 0xA9A0 / 0xA9B8 / 0xA430 failures assert-exit (not modelled).
// Lookup: game[0x1720 + (id & 0x7f) * 4] chain through item[0xE4] on item[0xC] == id; the item needs type 4 and mode item[0x10] == 0, else 0.
#include "d2game_seam4e.h"
static const uint32_t kFile = 0xB20FB14Cu;   // the source-file string at D2Game+0xFB14C, recorded as a module address
// 1.13c D2Game+0xD57F0
extern "C" uint32_t d2game_d57f0(uint32_t player, uint32_t game, uint32_t id, uint32_t slot, const D2Seam* e)
{
    if (!game || !player) return 0;
    e->call(0xD29F0, 0, 0, 0, 0, 0, player);
    uint32_t it = d2rd32(game + 0x1720 + (id & 0x7F) * 4);
    while (it && d2rd32(it + 0xC) != id) it = d2rd32(it + 0xE4);
    if (!it || d2rd32(it) != 4 || d2rd32(it + 0x10) != 0) return 0;
    if (!e->call(0xA934, 0, 0, 0, 0, 0, it, player, 0, 0, 0, 0)) return 0;
    const uint32_t inv = d2rd32(player + 0x60);
    const uint32_t r = e->call(0xA748, 0, 0, 0, 0, 0, inv, it);
    if (!r || r != it) return 0;
    const uint32_t s = e->call(0xA634, 0, 0, 0, 0, 0, player, 0, d2rd32(game + 0x70));
    const uint32_t v30 = e->call(0x1130, 0, it, 0, 0, 0);
    const uint32_t v00 = e->call(0x1100, 0, it, 0, 0, 0);
    e->call(0xA5E6, 0, 0, 0, 0, 0, kFile, 0x1516, inv, v00, v30, s, 0, 0);
    e->call(0xA92E, 0, 0, 0, 0, 0, it, 0xFF);
    if (!e->call(0xA9A0, 0, 0, 0, 0, 0, inv, it, slot & 0xFF)) return 0;
    if (!e->call(0xA9B8, 0, 0, 0, 0, 0, inv, it, 3)) return 0;
    e->call(0xA86E, 0, 0, 0, 0, 0, it, slot);
    e->call(0xA910, 0, 0, 0, 0, 0, inv, it);
    e->call(0xAA54, 0, 0, 0, 0, 0, inv, 0);
    if (e->call(0xA430, 0, 0, 0, 0, 0, it, 0xD)) return 0;
    e->call(0xD47C0, 0, 0, 0, 0, 0, 0);
    d2wr32(it + 0xC4, d2rd32(it + 0xC4) & 0xFFFFFFFDu);
    e->call(0xA4A2, 0, 0, 0, 0, 0, it, 1);
    e->call(0xA92E, 0, 0, 0, 0, 0, it, 0xFF);
    e->call(0xA916, 0, 0, 0, 0, 0, it, 0x8000, 1);
    if (e->call(0xD1BC0, it, 0, 0, 0, 0)) e->call(0xA65E, 0, 0, 0, 0, 0, it, 1, 1);
    if (e->call(0xA8A4, 0, 0, 0, 0, 0, it, 0x4000, 0x152B, kFile)) e->call(0xA65E, 0, 0, 0, 0, 0, it, 0x4000, 0);
    e->call(0xA994, 0, 0, 0, 0, 0, inv, it);
    e->call(0xD28D0, player, 0, 0, 0, 0);
    e->call(0xA9A6, 0, 0, 0, 0, 0, player, 1);
    e->call(0xD51A0, 0, 0, 0, 0, 0, game, player, 0);
    return 1;
}
