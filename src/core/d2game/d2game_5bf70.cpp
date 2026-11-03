// D2Game+0x5BF70 -- ACT5Q5_RewardPlayer: EAX = player unit, stack game (ret 4). Written from the 1.13c disassembly; callees are the D2Seam.
// 0x34C00 (EAX = unit, stack game), thunks 0xA48A (class to max level), 0xA42A (unit, 12, 0: level), 0xA3D6 (class, level: threshold),
// 0xA3E2 (unit, stat, 0: base stat), 0xA472 (unit, stat, value, 0: set), 0xEB090 (ESI = unit, stack game: level-up).
#include "d2game_seam4e.h"
// 1.13c D2Game+0x5BF70
extern "C" void d2game_5bf70(uint32_t unit, uint32_t game, const D2Seam* e)
{
    e->call(0x34C00, unit, 0, 0, 0, 0, game);
    int32_t ex = 1400000;
    const uint32_t diff = d2rd8(game + 0x6D);
    if (diff == 1) ex = 20000000;
    else if (diff == 2) ex = 40000000;
    const uint32_t cls = unit ? d2rd32(unit + 4) : 0xFFFFFFFFu;
    const int32_t maxLvl = (int32_t)e->call(0xA48A, 0, 0, 0, 0, 0, cls);
    int32_t lvl = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0xC, 0);
    if (lvl >= maxLvl) return;
    const int32_t hi = (int32_t)e->call(0xA3D6, 0, 0, 0, 0, 0, cls, (uint32_t)lvl + 1);
    const int32_t lo = (int32_t)e->call(0xA3D6, 0, 0, 0, 0, 0, cls, (uint32_t)lvl);
    if (ex > hi - lo) ex = hi - lo;
    for (;;) {
        if (!ex) return;
        const uint32_t next = e->call(0xA3E2, 0, 0, 0, 0, 0, unit, 0x1E, 0);
        const uint32_t cur = e->call(0xA3E2, 0, 0, 0, 0, 0, unit, 0xD, 0);
        const uint32_t d = next - cur;
        if ((uint32_t)ex < d) {
            const uint32_t c2 = e->call(0xA3E2, 0, 0, 0, 0, 0, unit, 0xD, 0);
            e->call(0xA472, 0, 0, 0, 0, 0, unit, 0xD, (uint32_t)ex + c2, 0);
            ex = 0;
        } else {
            e->call(0xA472, 0, 0, 0, 0, 0, unit, 0xD, next, 0);
            e->call(0xA472, 0, 0, 0, 0, 0, unit, 0x1D, d, 0);
            e->call(0xEB090, 0, 0, 0, 0, unit, game);
            ex -= (int32_t)d;
            lvl = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0xC, 0);
        }
        if (lvl >= maxLvl) return;
    }
}
