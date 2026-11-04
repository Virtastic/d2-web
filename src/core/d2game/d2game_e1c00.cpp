// D2Game+0xE1C00 (EAX = source warp tile, ESI = player, stack a0 = game, ret 4): put a player through a warp tile.
// The tile's room and class id select the destination warp tile; a free spot is searched around it (the search may move the
// coordinate), the quest gate runs for five destination levels, the placement helper moves the player, then the walk-in
// step and the client's position packet (type 0xD) go out with the warp record's exit offsets added.
// A null game or player, or a target that is not a player, is the common fatal (the process ends); this port just returns.
#include <cstdint>
#include "d2game_seam4e.h"

extern "C" void d2game_e1c00(uint32_t tile, uint32_t player, uint32_t game, const D2Seam* e)
{
    if (!game || !player || d2rd32(player) != 0) return;
    const uint32_t tilesRoom = e->call(0xA478, 0, 0, 0, 0, 0, tile);
    const uint32_t classId = tile ? d2rd32(tile + 4) : 0xFFFFFFFFu;
    uint32_t warpRec = 0;
    const uint32_t warpTile = e->call(0xA610, 0, 0, 0, 0, 0, tilesRoom, classId, (uint32_t)(uintptr_t)&warpRec);
    if (!warpTile) return;
    const uint32_t warpRoom = e->call(0xA478, 0, 0, 0, 0, 0, warpTile);
    uint32_t coord[2];
    coord[0] = e->call(0x1100, 0, warpTile, 0, 0, 0);
    coord[1] = e->call(0x1130, 0, warpTile, 0, 0, 0);
    const uint32_t size = e->call(0xA71E, 0, 0, 0, 0, 0, player);
    const uint32_t destRoom = e->call(0xA694, 0, 0, 0, 0, 0, warpRoom, (uint32_t)(uintptr_t)coord, size, 0x1C09, 1);
    if (!destRoom) return;
    const uint32_t srcLevel = e->call(0xA556, 0, 0, 0, 0, 0, tilesRoom);
    const uint32_t destLevel = e->call(0xA556, 0, 0, 0, 0, 0, destRoom);
    if (destLevel == 0x49 || destLevel == 0x80 || destLevel == 0x84 || destLevel == 0x76 || destLevel == 0x64)
        if (e->call(0x34020, game, destLevel, 0, 0, 0, player, srcLevel)) return;
    const uint32_t x = coord[0], y = coord[1];
    if (!e->call(0xE19C0, y, x, warpRoom, 0, 0, game, player, 0, 0)) return;
    const uint32_t client = d2rd32(player) == 0 ? d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, player) + 0x9C) : 0;
    const uint32_t nx = d2rd32(warpRec + 0x14) + x, ny = d2rd32(warpRec + 0x18) + y;
    e->call(0x78550, 0, y, player, 0, 0, 0, 2, nx, ny, 0);
    const uint32_t flag = e->call(0xA718, 0, 0, 0, 0, 0, player) & 0xFF;
    e->call(0x8C280, d2rd32(player), 0xD, d2rd32(player + 0xC), 0, 0, client, 1, nx, ny, 0, flag);
}
