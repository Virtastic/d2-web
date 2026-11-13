// 1.13c D2Game+0xB9290 -- writes the quest 0x17 progress into the player flags of quest 0x1a
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fb9290(NE& e, uint32_t game, uint32_t unit, uint32_t s0)
{
    const uint32_t buf = e.qbuf(game, unit);
    if (e.fl(buf, 0x1a, 0) == 1) return 0;
    const uint32_t n = e.quest(game, 0x17);
    if (!n) return 0;
    const uint32_t c = e.rd8(n + 0xc);
    if (c == 2) e.setf(buf, 0x1a, 2);
    else if (c == 3) e.setf(buf, 0x1a, e.rd8(n + 0xb) == 1 ? 3 : 4);
    return 0;
}
}
