// D2Game+0xD5740 equip-item tail. EAX = item, ECX = unit, EDX = game, plain ret. Written from the 1.13c disassembly.
// 0xA65E(item, 0x100, 1), 0xA946(inv, item) with inv = [unit + 0x60]; when 0xA988(item) == 1: 0xD46B0 (ESI = unit, EDI = item, stack game, 1, 1).
// 0xD51A0(game, unit, 0), 0xA994(inv, item), 0xA9A6(unit, 1), 0xA472(item, 0x48, 0, 0). Client id v = [[0xA4C0(unit)] + 0x9C] when [unit] == 0, else 0:
// 0x8B010 (EAX = item, EBX = 0, stack v, 1, 0x48, 0). 0xA96A(unit, item, 1, 1); [unit + 0x6E] = 9, [unit + 0x70] = unit, 0xA4DE(unit), [unit + 0xC4] |= 0x400.
#include "d2game_seam4e.h"
// 1.13c D2Game+0xD5740
extern "C" void d2game_d5740(uint32_t item, uint32_t unit, uint32_t game, const D2Seam* e)
{
    const uint32_t inv = d2rd32(unit + 0x60);
    e->call(0xA65E, 0, 0, 0, 0, 0, item, 0x100, 1);
    e->call(0xA946, 0, 0, 0, 0, 0, d2rd32(unit + 0x60), item);
    if (e->call(0xA988, 0, 0, 0, 0, 0, item) == 1) {
        e->call(0xD46B0, 0, 0, 0, 0, unit, game, 1, 1);
        e->edi(0xD46B0, item);
    }
    e->call(0xD51A0, 0, 0, 0, 0, 0, game, unit, 0);
    e->call(0xA994, 0, 0, 0, 0, 0, inv, item);
    e->call(0xA9A6, 0, 0, 0, 0, 0, unit, 1);
    e->call(0xA472, 0, 0, 0, 0, 0, item, 0x48, 0, 0);
    uint32_t v = 0;
    if (d2rd32(unit) == 0) v = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9C);
    e->call(0x8B010, item, 0, 0, 0, 0, v, 1, 0x48, 0);
    e->call(0xA96A, 0, 0, 0, 0, 0, unit, item, 1, 1);
    d2wr16(unit + 0x6E, 9);
    d2wr32(unit + 0x70, unit);
    e->call(0xA4DE, 0, 0, 0, 0, 0, unit);
    d2wr32(unit + 0xC4, d2rd32(unit + 0xC4) | 0x400);
}
