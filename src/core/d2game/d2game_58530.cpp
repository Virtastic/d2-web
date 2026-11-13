// 1.13c D2Game+0x58530 -- per-player copy of the quest 0x20 state into the flags of quest 0x24
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f58530(NE& e, uint32_t game, uint32_t unit, uint32_t s0)
{
    const uint32_t buf = e.qbuf(game, unit);
    if (e.fl(buf, 0x24, 0) == 1 || e.fl(buf, 0x24, 1) == 1) return 0;
    const uint32_t n = e.quest(game, 0x20);
    if (!n) return 0;
    const uint32_t st = e.rd8(n + 0xc);
    if (st == 2) e.setf(buf, 0x24, 2);
    else if (st == 3) e.setf(buf, 0x24, 3);
    return 0;
}
}
