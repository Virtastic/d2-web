// 1.13c D2Game+0x6E150 -- unit state event (fastcall ECX = game, EDX = unit, may be null): rec = 0xA520(unit ? unit[1] : -1); state = unit ? unit[4] : 0.
// State 1 sets the next event 0x8EA80(game, unit, 9, game[0xA8] + 10, 0, 0, 0). State 2 bumps the counter byte at unit[5][4]; a result below 2 runs 0xA478(unit), 0x6AE60(game) and
// 0x8EA80(..., game[0xA8] + 0x19, ...); otherwise it asks 0x6B570({game, unit, 0, game[0x10F0], unit[1]}, 0), and on a nonzero answer 0xA4A2(unit, 3) and
// 0x8EA80(..., (rec[0xE4] >> 8) + 1 + game[0xA8], ...). State 3 does 0xA4A2(unit, 4). Void.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
void f6e150(NE& e, uint32_t game, uint32_t unit)
{
    uint32_t st = 0, id = 0xFFFFFFFFu;
    if (unit) { st = e.rd32(unit + 0x10); id = e.rd32(unit + 4); }
    const uint32_t rec = e.imp(0xA520, id);
    if (st == 1) {
        e.h(0x8EA80, 0, game, unit, 0, 0, 9, e.rd32(game + 0xA8) + 10, 0, 0, 0);
    } else if (st == 2) {
        const uint32_t ctx = e.rd32(unit + 0x14);
        const uint8_t b = (uint8_t)(e.rd8(ctx + 4) + 1);
        e.wr8(ctx + 4, b);
        if (b <= 1) {
            e.imp(0xA478, unit);
            e.imp(0x6AE60, game);
            e.h(0x8EA80, 0, game, unit, 0, 0, 9, e.rd32(game + 0xA8) + 0x19, 0, 0, 0);
            return;
        }
        e.imp(0xA478, unit);
        uint32_t s[5] = { game, unit, 0, e.rd32(game + 0x10F0), e.rd32(unit + 4) };
        const uint32_t pre[5] = { s[0], s[1], s[2], s[3], s[4] };
        const uint32_t ok = e.imp(0x6B570, e.stk(s), 0);
        e.blk(0x6B570, pre, 5);
        if (!ok) return;
        e.imp(0xA4A2, unit, 3);
        e.h(0x8EA80, 0, game, unit, 0, 0, 9, (e.rd32(rec + 0xE4) >> 8) + 1 + e.rd32(game + 0xA8), 0, 0, 0);
    } else if (st == 3) {
        e.imp(0xA4A2, unit, 4);
    }
}
}
