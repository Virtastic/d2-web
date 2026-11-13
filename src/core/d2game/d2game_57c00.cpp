// 1.13c D2Game+0x57C00 -- per-player copy of the quest 0x22 state into the flags of quest 0x26
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f57c00(NE& e, uint32_t game, uint32_t unit, uint32_t s0)
{
    const uint32_t buf = e.qbuf(game, unit);
    if (!e.fl(buf, 0x25, 0) && !e.fl(buf, 0x25, 1)) return 0;
    if (e.fl(buf, 0x26, 0) == 1 || e.fl(buf, 0x26, 1) == 1) return 0;
    const uint32_t n = e.quest(game, 0x22);
    if (!n) return 0;
    const uint32_t st = e.rd8(n + 0xc);
    if (st == 2) e.setf(buf, 0x26, 2);
    else if (st == 3) e.setf(buf, 0x26, 3);
    return 0;
}
}
