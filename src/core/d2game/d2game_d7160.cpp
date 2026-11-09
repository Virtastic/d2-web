// D2Game+0xD7160: take an item off a unit's inventory into the cursor. EAX = unit, EBX = game, stack a0 = item id, a1 = out cell (ret 8).
// Written from the 1.13c disassembly. game == 0, unit == 0 or inv == 0 assert-exit (not modelled).
// *out = 0; inv = [unit + 0x60]; 0xA6A6(inv) != 0 or 0x37680 (ESI = unit) != 0: return. 0xD29F0(unit). Lookup game[0x1720 + (id & 0x7F) * 4] chain through item[0xE4]
// on item[0xC] == id; type item[0] must be 4; item[0x10] != 3: *out = 1. 0xD2F00(item, game) == 0: 0xD2840 (ESI = item, stack 0x13).
// 0xA44E(0xA478(item), item[0], item[0xC]); 0xAA66(item) == 4: 0xD6C80 (ESI = unit, stack game, item), 0xDFFD0 (ESI = unit, stack 1, unit).
// Else 0xA550(item), 0xA63A(item), item[0xC4] &= ~2, 0xAA54(inv, item), 0xA4A2(item, 4), 0xA916(item, 0x40, 1), 0xA92E(item, 0xFF), 0xA9A6(unit, 1),
// item[0xC4] &= ~0x2000000, 0xA994(inv, item), 0x345A0 (EAX = item, ECX = game, EDX = unit), 0xDFFD0 (ESI = unit, stack 1, unit).
#include "d2game_seam4e.h"
// 1.13c D2Game+0xD7160
extern "C" void d2game_d7160(uint32_t unit, uint32_t game, uint32_t id, uint32_t out, const D2Seam* e)
{
    if (!game || !unit) return;
    d2wr32(out, 0);
    const uint32_t inv = d2rd32(unit + 0x60);
    if (!inv) return;
    if (e->call(0xA6A6, 0, 0, 0, 0, 0, inv)) return;
    if (e->call(0x37680, 0, 0, 0, 0, unit)) return;
    e->call(0xD29F0, 0, 0, 0, 0, 0, unit);
    uint32_t it = d2rd32(game + 0x1720 + (id & 0x7F) * 4);
    while (it && d2rd32(it + 0xC) != id) it = d2rd32(it + 0xE4);
    if (!it || d2rd32(it) != 4) return;
    if (d2rd32(it + 0x10) != 3) { d2wr32(out, 1); return; }
    if (!e->call(0xD2F00, 0, 0, 0, 0, 0, game, it)) { e->call(0xD2840, 0, 0, 0, 0, it, 0x13); return; }
    e->call(0xA44E, 0, 0, 0, 0, 0, e->call(0xA478, 0, 0, 0, 0, 0, it), d2rd32(it), d2rd32(it + 0xC));
    if (e->call(0xAA66, 0, 0, 0, 0, 0, it) == 4) {
        e->call(0xD6C80, 0, 0, 0, 0, unit, game, it);
        e->call(0xDFFD0, 0, 0, 0, 0, unit, 1, unit);
        return;
    }
    e->call(0xA550, 0, 0, 0, 0, 0, it);
    e->call(0xA63A, 0, 0, 0, 0, 0, it);
    d2wr32(it + 0xC4, d2rd32(it + 0xC4) & 0xFFFFFFFDu);
    e->call(0xAA54, 0, 0, 0, 0, 0, inv, it);
    e->call(0xA4A2, 0, 0, 0, 0, 0, it, 4);
    e->call(0xA916, 0, 0, 0, 0, 0, it, 0x40, 1);
    e->call(0xA92E, 0, 0, 0, 0, 0, it, 0xFF);
    e->call(0xA9A6, 0, 0, 0, 0, 0, unit, 1);
    d2wr32(it + 0xC4, d2rd32(it + 0xC4) & 0xFDFFFFFFu);
    e->call(0xA994, 0, 0, 0, 0, 0, inv, it);
    e->call(0x345A0, it, game, unit, 0, 0);
    e->call(0xDFFD0, 0, 0, 0, 0, unit, 1, unit);
}
