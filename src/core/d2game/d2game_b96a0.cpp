// 1.13c D2Game+0xB96A0 -- writes the quest 0x16 progress into the player flags of quest 0x19
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fb96a0(NE& e, uint32_t game, uint32_t unit, uint32_t s0)
{
    const uint32_t buf = e.qbuf(game, unit);
    if (e.fl(buf, 0x19, 0) == 1) return 0;
    const uint32_t n = e.quest(game, 0x16);
    if (!n) return 0;
    const uint32_t c = e.rd8(n + 0xc);
    if (c == 2) e.setf(buf, 0x19, 2);
    else if (c == 3) e.setf(buf, 0x19, e.rd8(e.rd32(n + 0x18) + 0xc) ? 4 : 3);
    return 0;
}
}
