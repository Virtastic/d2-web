// 1.13c D2Game+0xB93B0 -- player enters a room: in or next to the quest 0x16 room the player flags 0xD and 1 of quest 0x19 are set
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fb93b0(NE& e, uint32_t game, uint32_t unit, uint32_t s0)
{
    const uint32_t room = e.imp(0xA478, unit);
    const uint32_t n = e.quest(game, 0x16);
    const uint32_t target = e.imp(0xA478, e.rd32(e.rd32(n + 0x18) + 0x14));
    if (!room) return 0;
    bool near = room == target;
    if (!near) {
        const uint32_t r = e.imp(0xA478, unit);
        uint32_t count = 0, list = 0;
        e.imp(0xA55C, r, e.stk(&list), e.stk(&count));
        for (uint32_t k = 0; k < count && !near; k++) near = e.rd32(list + 4 * k) == target;
    }
    const uint32_t buf = e.qbuf(game, unit);
    if (e.fl(buf, 0x19, 0)) return 0;
    if (e.fl(buf, 0x19, 1)) return 0;
    if (!near) return 0;
    e.setf(buf, 0x19, 0xd);
    e.setf(buf, 0x19, 1);
    return 0;
}
}
