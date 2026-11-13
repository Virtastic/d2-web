// 1.13c D2Game+0x630D0 -- player leaves the level 0x25 room: drops the player from the three quest 6 lists and resends the state
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f630d0(NE& e, uint32_t game, uint32_t unit, uint32_t s0)
{
    const uint32_t buf = e.qbuf(game, unit);
    const uint32_t room = e.imp(0xA478, unit);
    if (e.fl(buf, 6, 0) == 1) return 0;
    if (e.fl(buf, 6, 0xf) == 1) return 0;
    if (!room || e.imp(0xA556, room) != 0x25) return 0;
    const uint32_t data = e.rd32(e.quest(game, 6) + 0x18), uid = (unit ? e.rd32(unit + 0xc) : 0xffffffffu);
    e.h(0x33880, data, 0, 0, 0, uid);
    e.h(0x33880, data + 0x84, 0, 0, 0, uid);
    e.h(0x33880, data + 0x108, 0, 0, 0, uid);
    e.h(0x62640, buf, unit, 0, 0, 0, game);
    return 0;
}
}
