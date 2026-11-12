// 1.13c D2Game+0xEAB60 -- launch missile 0x170 from the unit's nearby object (record 0x182). EBX = unit, stack game, ret 4.
// Callees through the seam: 0xA478 (unit to room / object), 0xA4F6 (object lookup), 0xA454 / 0xA3B8 (path x / y), 0x6F930 (create the
// missile from a 23-dword block, ECX = game, EDX = block), 0xA50E, 0xA51A, 0xA4A2, 0xA520 (table row), 0x8EA80 (post event 1).
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_eab60(uint32_t unit, uint32_t game, const D2Seam* e)
{
    const uint32_t room = e->call(0xA478, 0, 0, 0, 0, 0, unit);
    const uint32_t obj = e->call(0xA4F6, 0, 0, 0, 0, 0, room, 2, 0x182);   // 0x6FD0AB69: (room, 2, 0x182) pushed before the 0xA478 call
    if (!obj) {
        const uint32_t s = e->call(0xA2DA, 0, 0, 0, 0, 0, 0x8F);
        e->call(0xA2C2, 0, 0, 0, 0, 0, 0x6FD19FF9, s);   // then _exit(-1)
        return 0;
    }
    uint32_t blk[23] = {};
    blk[0] = 0x420; blk[1] = unit; blk[2] = unit; blk[4] = 0x170;
    const uint32_t type = d2rd32(obj), path = d2rd32(obj + 0x2C);
    const bool onPath = type == 2 || (type > 3 && type < 6);
    blk[7] = onPath ? d2rd32(path + 0xC) : path ? e->call(0xA454, 0, 0, 0, 0, 0, path) : 0;
    blk[8] = onPath ? d2rd32(path + 0x10) : path ? e->call(0xA3B8, 0, 0, 0, 0, 0, path) : 0;
    blk[12] = 1;
    const uint32_t b = (uint32_t)(uintptr_t)blk;
    const uint32_t m = e->call(0x6F930, 0, game, b);
    e->blk(0x6F930, blk, 23);
    if (m) {
        e->call(0xA50E, 0, 0, 0, 0, 0, m, d2rd32(obj + 0xC));
        const uint32_t r2 = e->call(0xA478, 0, 0, 0, 0, 0, obj);
        return e->call(0xA51A, 0, 0, 0, 0, 0, r2, 0);
    }
    const uint32_t flag = d2rd32(obj + 0x10);
    if (flag) return flag;
    e->call(0xA4A2, 0, 0, 0, 0, 0, obj, 1);
    const uint32_t rec = e->call(0xA520, 0, 0, 0, 0, 0, 0x182);
    return e->call(0x8EA80, 0, game, obj, 0, 0, 1, (d2rd32(rec + 0xDC) >> 8) + d2rd32(game + 0xA8), 0, 0, 0);
}
