// D2Game+0xD8600 -- equip the cursor item into a body location (belt row / slot <= 10) and displace the occupant. EAX = item id, ECX = player,
// stack a0 game, a1 slot (byte), a2 flag, a3 *err (ret 0x10). Written from the 1.13c disassembly. game / player == 0 and the A748 asserts exit the
// process (not modelled). The slot byte >= 0xB returns 0 before *err is touched; 0xAA78(player, slot, item, flag) must be 5; the occupant from
// 0xA7AE / 0xA850 (written through a stack local) must have mode 1.
#include "d2game_seam4e.h"
static const uint32_t kFile = 0xB20FB14Cu;   // the source-file string at D2Game+0xFB14C, recorded as a module address
// 1.13c D2Game+0xD8600
extern "C" uint32_t d2game_d8600(uint32_t id, uint32_t player, uint32_t game, uint32_t p1, uint32_t flag, uint32_t pOut, const D2Seam* e)
{
    if (!game || !player) return 0;
    const uint32_t slot = p1 & 0xFF;
    if (slot >= 0xB) return 0;
    d2wr32(pOut, 0);
    e->call(0xD29F0, 0, 0, 0, 0, 0, player);
    uint32_t it = d2rd32(game + 0x1720 + (id & 0x7F) * 4);
    while (it && d2rd32(it + 0xC) != id) it = d2rd32(it + 0xE4);
    if (!it) return 0;
    if (d2rd32(it) != 4) return 0;
    if (d2rd32(it + 0x10) != 4) return 0;
    if (e->call(0xAA78, 0, 0, 0, 0, 0, player, slot, it, flag) != 5) return 0;
    const uint32_t inv = d2rd32(player + 0x60);
    const uint32_t bag = e->call(0xA54A, 0, 0, 0, 0, 0, inv);
    if (bag) e->call(0xA9BE, 0, 0, 0, 0, 0, player, bag);
    uint32_t other = e->call(0xA7AE, 0, 0, 0, 0, 0, inv, slot);
    e->call(0xA850, 0, 0, 0, 0, 0, player, inv, (uint32_t)(uintptr_t)&other, slot);
    if (!other || d2rd32(other + 0x10) != 1) { d2wr32(pOut, 1); return 0; }
    e->call(0xD46B0, 0, 0, 0, 0, player, game, 0, 1); e->edi(0xD46B0, other);
    if (!flag && !e->call(0xA934, 0, 0, 0, 0, 0, it, player, 0, 0, 0, 0)) {
        e->call(0xD47C0, 0, 0, 0, 0, 0, 0);
        e->call(0xDFFD0, 0, 0, 0, 0, player, 0x14, 0);
        return 0;
    }
    if (e->call(0xAA66, 0, 0, 0, 0, 0, it) == 0x13) e->call(0xD7E30, it, 0, 0, 0, 0, game, player);
    e->call(0xA8AA, 0, 0, 0, 0, 0, player, other);
    e->call(0xA946, 0, 0, 0, 0, 0, inv, other);
    const uint32_t r = e->call(0xA748, 0, 0, 0, 0, 0, inv, other);
    if (!r || r != other) return 0;
    e->call(0xA742, 0, 0, 0, 0, 0, inv, slot);
    e->call(0xAA54, 0, 0, 0, 0, 0, inv, other);
    d2wr32(other + 0xC4, d2rd32(other + 0xC4) & 0xFFFFFFFDu);
    e->call(0xA4A2, 0, 0, 0, 0, 0, other, 4);
    e->call(0xA65E, 0, 0, 0, 0, 0, other, 0x80, 1);
    e->call(0xA916, 0, 0, 0, 0, 0, other, 0x20, 1);
    e->call(0xA65E, 0, 0, 0, 0, 0, other, 1, 1);
    if (e->call(0xA8A4, 0, 0, 0, 0, 0, other, 0x4000, 0xCF8, kFile)) e->call(0xA65E, 0, 0, 0, 0, 0, other, 0x4000, 0);
    e->call(0xA994, 0, 0, 0, 0, 0, inv, other);
    if (!e->call(0xA9A0, 0, 0, 0, 0, 0, inv, it, slot)) { d2wr32(pOut, 1); return 0; }
    if (!e->call(0xA9B8, 0, 0, 0, 0, 0, inv, it, 3)) { d2wr32(pOut, 1); return 0; }
    e->call(0xA86E, 0, 0, 0, 0, 0, it, p1);
    e->call(0xA910, 0, 0, 0, 0, 0, inv, it);
    e->call(0xD47C0, 0, 0, 0, 0, 0, 0);
    d2wr32(it + 0xC4, d2rd32(it + 0xC4) & 0xFFFFFFFDu);
    e->call(0xA4A2, 0, 0, 0, 0, 0, it, 1);
    e->call(0xA92E, 0, 0, 0, 0, 0, it, 0xFF);
    e->call(0xA65E, 0, 0, 0, 0, 0, it, 0x40, 1);
    e->call(0xA65E, 0, 0, 0, 0, 0, it, 1, 1);
    e->call(0xA916, 0, 0, 0, 0, 0, it, 0x20, 1);
    e->call(0xA65E, 0, 0, 0, 0, 0, it, 1, 1);
    if (e->call(0xA8A4, 0, 0, 0, 0, 0, it, 0x4000, 0xD13, kFile)) e->call(0xA65E, 0, 0, 0, 0, 0, it, 0x4000, 0);
    e->call(0xA994, 0, 0, 0, 0, 0, inv, it);
    e->call(0xD28D0, player);
    e->call(0xD51A0, 0, 0, 0, 0, 0, game, player, 0);
    return 1;
}
