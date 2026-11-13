// 1.13c D2Game+0x75160 -- unit event (fastcall ECX = game, EDX = block, block[1] = unit, may be null): the unit's class id (unit ? unit[1] : -1) must pass 0x1560(EAX id, EDX 3) and
// 0x1560(EAX id, EDX 0xD), else 0. For a unit with unit[0x10] != 0: p = unit[0x2C]; 0xAAFC(p, 8); 0x18A0(EAX unit) picks 0xA when it is 0x4E (and inside sgptDataTables +0xA80), else 5, and
// 0xAD24(p, v); 0xAAF0(p, unit, 0); 0xDA750(game; EDI = unit); 0xA4A2(unit, 0xD). Returns 1.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f75160(NE& e, uint32_t dt, uint32_t game, uint32_t ctx)
{
    const uint32_t unit = e.rd32(ctx + 4);
    const uint32_t id = unit ? e.rd32(unit + 4) : 0xFFFFFFFFu;
    if (!e.h(0x1560, id, 0, 3)) return 0;
    if (!e.h(0x1560, id, 0, 0xD)) return 0;
    if (unit && e.rd32(unit + 0x10)) {
        const uint32_t p = e.rd32(unit + 0x2C);
        e.imp(0xAAFC, p, 8);
        const int32_t k = (int32_t)e.h(0x18A0, unit);
        const uint32_t v = (k >= 0 && k < (int32_t)e.rd32(dt + 0xA80) && k == 0x4E) ? 0xA : 5;
        e.imp(0xAD24, p, v);
        e.imp(0xAAF0, p, unit, 0);
        e.imp(0xDA750, game);
        e.imp(0xA4A2, unit, 0xD);
    }
    return 1;
}
}
