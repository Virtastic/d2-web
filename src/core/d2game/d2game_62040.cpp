// 1.13c D2Game+0x62040 -- writes the quest 2 progress into the player flags 2, 3 or 4
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f62040(NE& e, uint32_t game, uint32_t unit, uint32_t s0)
{
    const uint32_t buf = e.qbuf(game, unit);
    if (e.fl(buf, 2, 0) == 1) return 0;
    if (e.fl(buf, 2, 1) == 1) return 0;
    const uint32_t n = e.quest(game, 2);
    if (!n) return 0;
    const uint32_t c = e.rd8(n + 0xc), b = e.rd8(n + 0xb);
    (void)b;
    if (c == 2) e.setf(buf, 2, 2);
    else if (c == 3) e.setf(buf, 2, b == 1 ? 3 : 4);
    return 0;
}
}
