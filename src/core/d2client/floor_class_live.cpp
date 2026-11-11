// The footstep floor class (1.13c D2Client+0x226A0 with its room-tile lookup 0x21ED0) on the live session's units.
// sub_226a0.cpp is the proved port over retail's 32-bit image; this runs the same steps on the engine's own structs:
//   room = the unit's room; x, y = its path subtile position.
//   0x21ED0: the first floor tile of the room whose (nPosX, nPosY) is ((x - room.x) / 5, (y - room.y) / 5) (signed,
//   truncating), else the same lookup over the room's near-room list in order.
//   0x21E80: the tile's library material word +6: 0x20 -> 1, 0x08 -> 2, 0x10 -> 3, 0x40 -> 4, 0x80 -> 6, 0x400 -> 5, else 0.
//   A tile class 0, or no tile, falls back to the level class: SoundEnviron "Material 1" (record +0x1C) of the local
//   player's level when it is 1..6, else 1. The caller passes that fallback (the table is not engine data).
// 0x22FB0 then adds 4 * (class - 1) to the footstep row for class 2..6; class 0 and 1 add nothing.
#include <cstdint>
#include <emscripten.h>
#include <D2Common.h>
#include <D2CMP.h>
#include <Drlg/D2DrlgDrlg.h>
#include <Units/Units.h>
#include <D2Game/UNIT/SUnit.h>

extern "C" D2UnitStrc* d2_session_target_player_unit();   // monspawn_check.cpp

namespace {
uint32_t tileClass(const D2DrlgTileDataStrc* t) {   // 0x21E80 on the engine's tile record
    if (!t->pTile) return 0;
    const uint16_t f = t->pTile->nFlags;
    if (f & 0x20) return 1;
    if (f & 0x08) return 2;
    if (f & 0x10) return 3;
    if (f & 0x40) return 4;
    if (f & 0x80) return 6;
    if (f & 0x400) return 5;
    return 0;
}
const D2DrlgTileDataStrc* roomTile(const D2ActiveRoomStrc* room, int32_t x, int32_t y) {   // 0x21ED0
    if (!room || !room->pRoomTiles) return nullptr;
    const int32_t tx = int32_t(uint32_t(x) - uint32_t(room->tCoords.nSubtileX)) / 5;
    const int32_t ty = int32_t(uint32_t(y) - uint32_t(room->tCoords.nSubtileY)) / 5;
    const D2DrlgTileDataStrc* tiles = room->pRoomTiles->pFloorTiles;
    for (int32_t i = 0; tiles && i < room->pRoomTiles->nFloors; ++i)
        if (tiles[i].nPosX == tx && tiles[i].nPosY == ty) return &tiles[i];
    return nullptr;
}
}

// unitType 0 = the local player, 1 = a monster by id. Returns the class 0..6 (0: no unit or room).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_session_floor_class(int32_t unitType, int32_t unitId, int32_t fallback) {
    D2UnitStrc* player = d2_session_target_player_unit();
    if (!player) return 0;
    D2UnitStrc* unit = unitType == 0 ? player : unitType == 1 && player->pGame ? SUNIT_GetServerUnit(player->pGame, UNIT_MONSTER, unitId) : nullptr;
    if (!unit || !unit->pDynamicPath) return 0;
    const int32_t ebp = fallback > 0 && fallback < 7 ? fallback : 1;
    D2ActiveRoomStrc* room = UNITS_GetRoom(unit);
    if (!room) return 0;
    const int32_t x = unit->pDynamicPath->tGameCoords.wPosX, y = unit->pDynamicPath->tGameCoords.wPosY;
    const D2DrlgTileDataStrc* rec = roomTile(room, x, y);
    for (int32_t i = 0; !rec && room->ppRoomList && i < room->nNumRooms; ++i) rec = roomTile(room->ppRoomList[i], x, y);
    if (!rec) return ebp;
    const uint32_t cls = tileClass(rec);
    return cls ? int32_t(cls) : ebp;
}
