// 1.13c D2Game+0x75460 -- unit event (fastcall ECX = game, EDX = block, block[1] = unit, may be null, block[5] byte): a unit with unit[0x10] != 0 runs 0x75350(EAX unit; block byte +0x14).
// Then 0xDA750(game; EBP game, EDI unit) and 0xA4A2(unit, 0xC). A unit's node list (head unit[0xDC], next +0x24) is walked twice: each node whose type byte is 8, then each whose type byte is 9, gets
// 0x8E6F0(EAX node; EDI game). Returns 1.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f75460(NE& e, uint32_t game, uint32_t ctx)
{
    const uint32_t unit = e.rd32(ctx + 4);
    if (unit && e.rd32(unit + 0x10)) e.h(0x75350, unit, 0, 0, 0, 0, e.rd8(ctx + 0x14));
    e.h(0xDA750, 0, 0, 0, 0, 0, game);
    e.y(0xDA750, game, unit);
    e.imp(0xA4A2, unit, 0xC);
    if (!unit) return 1;
    for (uint32_t n = e.rd32(unit + 0xDC); n;) {
        const uint32_t ty = e.rd8(n), nx = e.rd32(n + 0x24);
        if (ty == 8) { e.h(0x8E6F0, n); e.y(0x8E6F0, game, game); }
        n = nx;
    }
    for (uint32_t n = e.rd32(unit + 0xDC); n;) {
        const uint32_t ty = e.rd8(n), nx = e.rd32(n + 0x24);
        if (ty == 9) { e.h(0x8E6F0, n); e.y(0x8E6F0, game, game); }
        n = nx;
    }
    return 1;
}
}
