// 1.13c D2Game+0x61DA0 -- writes the quest 3 progress into the player flags 2 or 3
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f61da0(NE& e, uint32_t game, uint32_t unit, uint32_t s0)
{
    const uint32_t buf = e.qbuf(game, unit);
    if (e.fl(buf, 3, 0) == 1) return 0;
    if (e.fl(buf, 3, 1) == 1) return 0;
    const uint32_t n = e.quest(game, 3);
    if (!n) return 0;
    const uint32_t c = e.rd8(n + 0xc), b = e.rd8(n + 0xb);
    (void)b;
    if (c == 2) e.setf(buf, 3, 2);
    else if (c == 3 || c == 4) e.setf(buf, 3, 3);
    return 0;
}
}
