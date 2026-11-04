// D2Game+0xD5690 RepairBrokenItem tail. EDI = item, ESI = unit (or null), stack a0 game (ret 4). Written from the 1.13c disassembly.
// inv = unit ? [unit + 0x60] : 0. 0xA65E(item, 0x200, 1), 0xA65E(item, 0x100, 0). inv: 0xD4FF0 (EAX = item, ECX = unit, stack game), 0xD51A0(game, unit, 0),
// 0xA994(inv, item), 0xA9A6(unit, 1). n = 0xAA00(item); n > 0: 0xA472(item, 0x48, n, 0), then with a unit: 0x8B010 (EAX = item, stack v, 1, 0x48, n) where
// v = [[0xA4C0(unit)] + 0x9C] when [unit] == 0, else 0. Always 0xA96A(unit, item, 0, 0); a unit then 0xD28D0 (EAX = unit).
#include "d2game_seam4e.h"
// 1.13c D2Game+0xD5690
extern "C" void d2game_d5690(uint32_t item, uint32_t unit, uint32_t game, const D2Seam* e)
{
    const uint32_t inv = unit ? d2rd32(unit + 0x60) : 0;
    e->call(0xA65E, 0, 0, 0, 0, 0, item, 0x200, 1);
    e->call(0xA65E, 0, 0, 0, 0, 0, item, 0x100, 0);
    if (inv) {
        e->call(0xD4FF0, item, unit, 0, 0, 0, game);
        e->call(0xD51A0, 0, 0, 0, 0, 0, game, unit, 0);
        e->call(0xA994, 0, 0, 0, 0, 0, inv, item);
        e->call(0xA9A6, 0, 0, 0, 0, 0, unit, 1);
    }
    const int32_t n = (int32_t)e->call(0xAA00, 0, 0, 0, 0, 0, item);
    if (n > 0) {
        e->call(0xA472, 0, 0, 0, 0, 0, item, 0x48, (uint32_t)n, 0);
        if (unit) {
            uint32_t v = 0;
            if (d2rd32(unit) == 0) v = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9C);
            e->call(0x8B010, item, 0, 0, 0, 0, v, 1, 0x48, (uint32_t)n);
        }
    }
    e->call(0xA96A, 0, 0, 0, 0, 0, unit, item, 0, 0);
    if (unit) e->call(0xD28D0, unit);
}
