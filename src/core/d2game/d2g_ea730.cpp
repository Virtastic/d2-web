// 1.13c D2Game+0xEA730 -- stagger the nearby players of a unit into the event queue. Stdcall, six stack words, ret 0x18:
// game, unit, radius, lo (minimum delay), hi (maximum delay), needFlag. hi == 0 or hi <= lo is a fatal error (code 0x20 / 0x21).
// Callees go through the seam: 0xA478 (unit to room), 0xA454 / 0xA3B8 (path x / y), 0xA502 (room area scan), 0xA4F0 (run it),
// 0xA514 / 0xA508 (player filters), 0x11D0 (roll, ECX = seed at unit+0x20, ESI = range), 0x8EA80 (post event 7), 0x232B0, 0xA4FC (free).
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_ea730(uint32_t game, uint32_t unit, uint32_t radius, uint32_t lo, uint32_t hi, uint32_t needFlag, const D2Seam* e)
{
    const uint32_t room = e->call(0xA478, 0, 0, 0, 0, 0, unit);
    const uint32_t type = d2rd32(unit), path = d2rd32(unit + 0x2C);
    const bool onPath = type == 2 || (type > 3 && type < 6);
    const uint32_t x = onPath ? d2rd32(path + 0xC) : path ? e->call(0xA454, 0, 0, 0, 0, 0, path) : 0;
    const uint32_t y = onPath ? d2rd32(path + 0x10) : path ? e->call(0xA3B8, 0, 0, 0, 0, 0, path) : 0;
    if (hi == 0 || (int32_t)hi <= (int32_t)lo) {
        const uint32_t s = e->call(0xA2DA, 0, 0, 0, 0, 0, hi == 0 ? 0x20 : 0x21);
        e->call(0xA2C2, 0, 0, 0, 0, 0, 0x6FD19FF9, s);   // then _exit(-1)
        return 0;
    }
    uint32_t query[14] = {}, result[8] = {};
    query[0] = 0x583; query[2] = unit; query[3] = x; query[4] = y; query[5] = radius;
    const uint32_t q = (uint32_t)(uintptr_t)query, res = (uint32_t)(uintptr_t)result;
    e->call(0xA502, 0, 0, 0, 0, 0, d2rd32(game + 0x1C), res, room, x, y, radius, 0, q);
    e->blk(0xA502, query, 14);
    e->call(0xA4F0, 0, 0, 0, 0, 0, res);
    const int32_t count = (int32_t)result[6];
    const uint32_t list = result[1];
    for (int32_t i = 0; i < count; ++i) {
        const uint32_t m = d2rd32(list + 4 * i);
        if (!m || d2rd32(m) != 1) continue;
        bool ok = e->call(0xA514, 0, 0, 0, 0, 0, m) == 0;
        if (needFlag && e->call(0xA508, 0, 0, 0, 0, 0, m) == 0) ok = false;
        if (d2rd32(m + 4) == 0x14D || !ok) continue;
        const uint32_t roll = e->call(0x11D0, 0, unit + 0x20, 0, 0, hi - lo);
        e->call(0x8EA80, 0, game, m, 0, 0, 7, d2rd32(game + 0xA8) + (roll + lo), 0, 0, 0);
        e->call(0x232B0, 0, 0, 0, 0x15, game, m, 0);
    }
    return e->call(0xA4FC, 0, 0, 0, 0, 0, res);
}
