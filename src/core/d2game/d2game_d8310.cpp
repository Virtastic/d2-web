// D2Game+0xD8310: drop a placed item back from the unit's cursor. EAX = item id, stack a0 = game, a1 = unit (ret 8). Written from the 1.13c disassembly.
// game == 0 or unit == 0 assert-exit (not modelled). 0xD29F0(unit); lookup game[0x1720 + (id & 0x7F) * 4] chain through item[0xE4] on item[0xC] == id; type
// item[0] and mode item[0x10] must both be 4. 0xA544(unit, &a), room = 0xA478(unit), room2 = 0xE930 (ESI = &a, stack room, &b, 1): 0 returns.
// 0xE990 (EAX = b[1], ECX = item, EBX = room2, stack game, unit, b[0]), 0xAA54([unit + 0x60], 0), 0xD1BC0 (EAX = item) gating 0xA65E(item, 1, 1),
// 0xA8A4(item, 0x4000, 0x85F, file) gating 0xA65E(item, 0x4000, 0), then 0xD78C0(game, unit, item).
#include "d2game_seam4e.h"
static const uint32_t kFile = 0xB20FB14Cu;   // the source-file string at D2Game+0xFB14C, recorded as a module address
// 1.13c D2Game+0xD8310
extern "C" void d2game_d8310(uint32_t id, uint32_t game, uint32_t unit, const D2Seam* e)
{
    if (!game || !unit) return;
    e->call(0xD29F0, 0, 0, 0, 0, 0, unit);
    uint32_t it = d2rd32(game + 0x1720 + (id & 0x7F) * 4);
    while (it && d2rd32(it + 0xC) != id) it = d2rd32(it + 0xE4);
    if (!it || d2rd32(it) != 4 || d2rd32(it + 0x10) != 4) return;
    uint32_t bufA[2] = {}, bufB[2] = {};
    e->call(0xA544, 0, 0, 0, 0, 0, unit, (uint32_t)(uintptr_t)bufA);
    const uint32_t room = e->call(0xA478, 0, 0, 0, 0, 0, unit);
    const uint32_t room2 = e->call(0xE930, 0, 0, 0, 0, (uint32_t)(uintptr_t)bufA, room, (uint32_t)(uintptr_t)bufB, 1);
    if (!room2) return;
    e->call(0xE990, bufB[1], it, 0, room2, 0, game, unit, bufB[0]);
    e->call(0xAA54, 0, 0, 0, 0, 0, d2rd32(unit + 0x60), 0);
    if (e->call(0xD1BC0, it, 0, 0, 0, 0)) e->call(0xA65E, 0, 0, 0, 0, 0, it, 1, 1);
    if (e->call(0xA8A4, 0, 0, 0, 0, 0, it, 0x4000, 0x85F, kFile)) e->call(0xA65E, 0, 0, 0, 0, 0, it, 0x4000, 0);
    e->call(0xD78C0, 0, 0, 0, 0, 0, game, unit, it);
}
