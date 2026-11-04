// D2Game+0xDE380 (stdcall, ten stack words, ret 0x28): flag the units around a missile that its owner may hit.
// (game, unit, x, y, size, damage, selfOk, ownerOwnerOk, callback, findFlags). Finds every unit within `size` of (x, y)
// in the missile's room (flags default 0x583), takes the missile's owner and the owner's owner, then for each found unit
// that is not one of those (unless allowed), passes the owner's hostility test and the 0x805 collision test, copies the
// 0x70-byte damage block and hands the unit to the damage-flag setter (when a callback is given, only if it accepts).
// Returns 1 when any unit was flagged, 0 otherwise (also 0 for a missile without a live owner).
#include <cstdint>
#include <cstring>
#include "d2game_seam4e.h"

extern "C" uint32_t d2game_de380(uint32_t game, uint32_t unit, uint32_t nX, uint32_t nY, uint32_t size, uint32_t damage,
                                 uint32_t selfOk, uint32_t ownerOwnerOk, uint32_t callback, uint32_t findFlags, const D2Seam* e)
{
    const uint32_t room = e->call(0xA478, 0, 0, 0, 0, 0, unit);
    const uint32_t room2 = e->call(0x1280, room, 0, 0, nX, 0);
    e->edi(0x1280, nY);
    uint32_t arg[14] = {};
    arg[0] = findFlags ? findFlags : 0x583;
    arg[3] = nX; arg[4] = nY; arg[5] = size;
    uint32_t data[7] = {};
    const uint32_t pData = (uint32_t)(uintptr_t)data, pArg = (uint32_t)(uintptr_t)arg;
    e->call(0xA502, 0, 0, 0, 0, 0, d2rd32(game + 0x1C), pData, room2, nX, nY, size, 0, pArg);
    e->blk(0xA502, arg, 14);
    e->call(0xA4F0, 0, 0, 0, 0, 0, pData);
    auto fail = [&]() -> uint32_t { e->call(0xA4FC, 0, 0, 0, 0, 0, pData); return 0; };
    if (!unit || !((d2rd32(unit + 0xC8) >> 10) & 1)) return fail();
    const uint32_t owner = e->call(0xE03A0, d2rd32(unit + 0x94), game, d2rd32(unit + 0x98));
    if (!owner) return fail();
    uint32_t ownerOwner = 0;
    if ((d2rd32(owner + 0xC8) >> 10) & 1) ownerOwner = e->call(0xE03A0, d2rd32(owner + 0x94), game, d2rd32(owner + 0x98));
    uint32_t hit = 0;
    const int32_t n = (int32_t)data[6];
    for (int32_t i = 0; i < n; ++i) {
        const uint32_t found = d2rd32(data[1] + 4 * (uint32_t)i);
        if (ownerOwner == found && !ownerOwnerOk) continue;
        if (owner == found && !selfOk) continue;
        if (!e->call(0xE0A60, 0, owner, 0, 0, 0, game)) continue;
        if (!e->call(0xA820, 0, 0, 0, 0, 0, nX, nY, found, 0x805)) continue;
        uint32_t copy[28];
        memcpy(copy, (const void*)(uintptr_t)damage, sizeof copy);
        if (callback && !e->callback(callback, game, unit, found)) continue;
        e->call(0x3AE10, 0, 0, 0, 0, 0, game, unit, found);
        e->edi(0x3AE10, (uint32_t)(uintptr_t)copy);
        e->blk(0x3AE10, copy, 28);
        hit = 1;
    }
    e->call(0xA4FC, 0, 0, 0, 0, 0, pData);
    return hit;
}
