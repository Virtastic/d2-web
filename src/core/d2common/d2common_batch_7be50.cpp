// Stage A. Prior art used: D2MOO DRLG warp/orth/vertex/grid routines and
// ITEMS_SetDefenseOrDamage; checked against each cached 1.13c decompile.
// Native links use pointers; retail coordinates, IDs and arithmetic remain 32-bit.
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include "Drlg/D2DrlgDrlg.h"
#include "Drlg/D2DrlgDrlgRoom.h"
#include "Drlg/D2DrlgDrlgGrid.h"
#include "Drlg/D2DrlgDrlgVer.h"
#include "D2Items.h"

static int32_t add32(int32_t a, int32_t b) { return int32_t(uint32_t(a) + uint32_t(b)); }
static int32_t mul32(int32_t a, int32_t b) { return int32_t(uint32_t(a) * uint32_t(b)); }

// 1.13c D2Common+0x7BE50
extern "C" void d2_fn_d2common_7be50(D2DrlgRoomStrc* room)
{
    if (room->fRoomStatus < 4 && room->wRoomsInList[room->fRoomStatus]) return;
    uint8_t status = 0;
    while (status < 4 && !room->wRoomsInList[status]) ++status;
    if (room->fRoomStatus == status) return;
    if (room->pStatusPrev && room->pStatusNext) {
        room->pStatusPrev->pStatusNext = room->pStatusNext;
        room->pStatusNext->pStatusPrev = room->pStatusPrev;
        room->pStatusNext = nullptr;
        room->pStatusPrev = nullptr;
    }
    if (status < 4) {
        auto* head = &room->pLevel->pDrlg->tStatusRoomsLists[status];
        room->pStatusNext = head;
        room->pStatusPrev = head->pStatusPrev;
        head->pStatusPrev->pStatusNext = room;
        head->pStatusPrev = room;
        room->fRoomStatus = status;
    }
}

// 1.13c D2Common+0x29F30
extern "C" void d2_fn_d2common_29f30(D2UnitStrc* item, int32_t stat)
{
    // ESI holds item; the switch selector is preserved from the caller.
    ITEMS_SetDefenseOrDamage(item, stat);
}

// 1.13c D2Common+0x67F50
// EAX is room, EDI is the caller's pool. Fog 10046 takes pool in ECX,
// allocation in EDX, then file/line/flags on the stack. This helper does not
// reset pointers/counters or free links/grid (the larger D2MOO helper does).
extern "C" void d2_fn_d2common_67f50(D2DrlgRoomStrc* room, void* pool,
    void (*freePool)(void*, void*, const char*, int, int) = FOG_FreePool)
{
    if (auto* grid = room->pTileGrid) {
        constexpr auto file = "..\\Source\\D2Common\\DRLG\\RoomTile.cpp";
        if (grid->pTiles.pWallTiles) freePool(pool, grid->pTiles.pWallTiles, file, 0x592, 0);
        if (grid->pTiles.pFloorTiles) freePool(pool, grid->pTiles.pFloorTiles, file, 0x593, 0);
        if (grid->pTiles.pRoofTiles) freePool(pool, grid->pTiles.pRoofTiles, file, 0x594, 0);
    }
}

// 1.13c D2Common+0x2D210
extern "C" void d2_fn_d2common_2d210(D2DrlgWarpStrc* warp, int32_t id, int32_t vis, int32_t dest)
{
    for (int32_t i = 0; i < 8; ++i) {
        if (warp->nVis[i] == vis) { warp->nWarp[i] = dest; return; }
    }
    if (id == -1) {
        for (int32_t i = 0; i < 8; ++i) {
            if (!warp->nVis[i] && warp->nWarp[i] == -1) { id = i; break; }
        }
    }
    // Retail also leaves id=-1 when full: address the fields relative to the
    // containing object (nVis[-1] is nLevel; nWarp[-1] is nVis[7]).
    if (id == -1) { warp->nLevel = vis; warp->nVis[7] = dest; return; }
    warp->nVis[id] = vis;
    warp->nWarp[id] = dest;
}

// 1.13c D2Common+0x2D730
extern "C" D2DrlgWarpStrc* d2_fn_d2common_2d730(D2DrlgStrc* drlg, int32_t level)
{
    return DRLG_GetDrlgWarpFromLevelId(drlg, level);
}

static int32_t orthDirection(const D2DrlgOrthStrc* orth)
{
    int32_t value; std::memcpy(&value, &orth->nDirection, sizeof(value)); return value;
}
extern "C" void D2Common_44B20(D2DrlgOrthStrc** head, D2DrlgOrthStrc* node, bool (*before)(const D2DrlgOrthStrc*, const D2DrlgOrthStrc*));
extern "C" bool D2Common_44B20_Before(const D2DrlgOrthStrc* a, const D2DrlgOrthStrc* b);

struct D2CommonPlacement13c { int32_t x, y, width, height, direction; };
static_assert(offsetof(D2CommonPlacement13c, direction) == 0x10);
// 1.13c D2Common+0x13F30
extern "C" void D2Common_13F30_Placement(const int32_t* origin, int32_t direction,
    D2CommonPlacement13c* out, int32_t width, int32_t height)
{
    int32_t dx = 0, dy = 0;
    switch (direction) {
    case 0: dy = int32_t(0u - uint32_t(height)); break;
    case 1: case 2: {
        int32_t hi = int32_t((int64_t(height) * 0x55555555LL) >> 32);
        dy = int32_t(uint32_t(hi) - uint32_t(height)) >> 1;
        dy = add32(dy, -(dy >> 31));
        dx = direction == 1 ? int32_t(0u - uint32_t(width)) : width;
        break;
    }
    case 3: case 4: {
        int32_t twice = mul32(height, -2);
        dy = add32(twice / 3, twice >> 31);
        dy = add32(dy, -(dy >> 31));
        dx = direction == 3 ? int32_t(0u - uint32_t(width)) : width;
        break;
    }
    }
    out->x = add32(origin[0], dx); out->y = add32(origin[1], dy);
    out->width = width; out->height = height; out->direction = direction;
}

// 1.13c D2Common+0x37020
extern "C" void d2_fn_d2common_37020(D2DrlgGridStrc* grid, D2DrlgVertexStrc* vertex,
    int32_t flag, FlagOperation operation, BOOL includeNext)
{
    sub_6FD75DE0(grid, vertex, flag, operation, includeNext);
}

