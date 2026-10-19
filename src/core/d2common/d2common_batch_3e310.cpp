// Prior art: D2MOO PathUtil, D2Inventory, D2StatList, ItemMods, DrlgOutPlace,
// DrlgOutRoom and DrlgDrlgRoom; checked against cached 1.13c bodies.
// Stage A: these definitions are not a vector parity claim.
#include <algorithm>
#include <bit>
#include <cstdint>
#include "D2Inventory.h"
#include "D2Items.h"
#include "D2StatList.h"
#include "D2DataTbls.h"
#include "D2ItemMods.h"
#include "Units/Units.h"
#include "Path/Path.h"
#include "Fog.h"
#include "Drlg/D2DrlgOutPlace.h"
#include "Drlg/D2DrlgOutdoors.h"
#include "Drlg/D2DrlgOutRoom.h"
#include "Drlg/D2DrlgRoomTile.h"
#include "Drlg/D2DrlgDrlgGrid.h"
#include "Drlg/D2DrlgDrlgLogic.h"
#include "Drlg/D2DrlgTileSub.h"
#include "DataTbls/LevelsIds.h"

// 1.13c D2Common+0x3E310
void D2Common_3E310_TargetPoint(D2PathPointStrc* out, D2DynamicPathStrc* path)
{
    D2_ASSERT(path);
    D2_ASSERT(path->pTargetUnit);
    auto* target = path->pTargetUnit;
    out->X = static_cast<uint16_t>(UNITS_GetXPosition(target));
    out->Y = static_cast<uint16_t>(UNITS_GetYPosition(target));
    if (path->dwSpeed) {
        const unsigned direction = target->pDynamicPath ? target->pDynamicPath->nDirection : 0;
        const auto angle = static_cast<uint16_t>(((direction - 32u * path->dwSpeed) & 63u) * 8u);
        out->X += static_cast<int64_t>(FOG_10083_Cos_LUT(angle) * static_cast<float>(path->dwSpeed));
        out->Y += static_cast<int64_t>(FOG_10084_Sin_LUT(angle) * static_cast<float>(path->dwSpeed));
    }
}

// 1.13c D2Common+0x38220
int D2Common_38220_LowerBound(D2StatsArrayStrc* stats, uint32_t key)
{
    int first = 0, last = static_cast<int16_t>(stats->nStatCount);
    const int count = last;
    while (first < last) {
        const int mid = first + (last - first) / 2;
        // Retail compares the packed value with signed x86 comparisons.
        const int32_t value = static_cast<int32_t>(stats->pStat[mid].nPackedValue);
        if (value < static_cast<int32_t>(key)) first = mid + 1;
        else if (value == static_cast<int32_t>(key)) return mid;
        else last = mid;
    }
    return first < count ? first : -1;
}

// 1.13c D2Common+0x38CD0
int32_t D2Common_38CD0_InitialStat(D2StatListStrc* list, uint32_t key, const D2ItemStatCostTxt* cost)
{
    auto find = [key](D2StatsArrayStrc& stats) -> uint32_t {
        const int index = D2Common_38220_LowerBound(&stats, key);
        return index >= 0 && stats.pStat[index].nPackedValue == key ? stats.pStat[index].nValue : 0;
    };
    uint32_t total = find(list->Stats);
    if (list->dwFlags & 0x80000000u) {
        for (auto* child = reinterpret_cast<D2StatListExStrc*>(list)->pMyLastList; child; child = child->pPrevLink) {
            if ((cost->dwItemStatFlags & 4) && (child->dwFlags & 0x40000000)) continue;
            auto& stats = (child->dwFlags & 0x80000000u) ? reinterpret_cast<D2StatListExStrc*>(child)->FullStats : child->Stats;
            total += find(stats);
        }
    }
    return static_cast<int32_t>(total);
}

// 1.13c D2Common+0x1F720
void D2Common_1F720_UnlinkGrid(D2UnitStrc* item, D2InventoryGridStrc* grid, int rectangular)
{
    auto* extra = INVENTORY_GetItemExtraDataFromItem(item);
    if (!extra) return;
    if (extra->pPreviousGridItem) {
        if (auto* previous = INVENTORY_GetItemExtraDataFromItem(extra->pPreviousGridItem)) previous->pNextGridItem = extra->pNextGridItem;
    } else if (grid->pItem == item) grid->pItem = extra->pNextGridItem;
    if (extra->pNextGridItem) {
        if (auto* next = INVENTORY_GetItemExtraDataFromItem(extra->pNextGridItem)) next->pPreviousGridItem = extra->pPreviousGridItem;
    } else if (grid->pLastItem == item) grid->pLastItem = extra->pPreviousGridItem;
    if (grid->ppItems) {
        const int x = UNITS_GetXPosition(item), y = UNITS_GetYPosition(item);
        if (!rectangular) grid->ppItems[y * grid->nGridWidth + x] = nullptr;
        else {
            // Missing-table fallback is the low byte of the retail grid address.
            uint8_t width = static_cast<uint8_t>(reinterpret_cast<uintptr_t>(grid)), height = width;
            if (const auto* record = DATATBLS_GetItemsTxtRecord(item->dwClassId)) {
                width = record->nInvWidth; height = record->nInvHeight;
            }
            for (int row = y; row < y + height; ++row)
                for (int col = x; col < x + width; ++col) grid->ppItems[row * grid->nGridWidth + col] = nullptr;
        }
    }
    extra->pNextGridItem = extra->pPreviousGridItem = nullptr;
    extra->nNodePos = 0;
}

// 1.13c D2Common+0x1F8F0
void D2Common_1F8F0_UnlinkInventory(D2UnitStrc* item, D2UnitStrc** endpoints)
{
    auto* extra = INVENTORY_GetItemExtraDataFromItem(item);
    if (!extra) return;
    if (extra->pPreviousItem) {
        if (auto* previous = INVENTORY_GetItemExtraDataFromItem(extra->pPreviousItem)) previous->pNextItem = extra->pNextItem;
    } else if (endpoints[0] == item) endpoints[0] = extra->pNextItem;
    if (extra->pNextItem) {
        if (auto* next = INVENTORY_GetItemExtraDataFromItem(extra->pNextItem)) next->pPreviousItem = extra->pPreviousItem;
    } else if (endpoints[1] == item) endpoints[1] = extra->pPreviousItem;
    extra->pNextItem = extra->pPreviousItem = nullptr;
}

// 1.13c D2Common+0x45F90
void D2Common_45F90_SetBonuses(D2UnitStrc* unit, D2UnitStrc* item, int state)
{
    if (!state || !item || item->dwUnitType != UNIT_ITEM || ITEMS_GetItemQuality(item) != ITEMQUAL_SET) return;
    const int index = ITEMS_GetFileIndex(item);
    if (index < 0 || index >= sgptDataTables->nSetItemsTxtRecordCount) return;
    const auto& setItem = sgptDataTables->pSetItemsTxt[index];
    if (setItem.nSetId < 0 || setItem.nSetId >= sgptDataTables->nSetsTxtRecordCount) return;
    auto& set = sgptDataTables->pSetsTxt[setItem.nSetId];
    const uint32_t mask = ITEMS_GetSetItemsMask(unit, item, 1);
    const int count = mask < 64 ? std::popcount(mask) : 0;
    const int partial = 2 * std::min(count, set.nSetItems - 1) - 2;
    for (int i = 0; i < partial; ++i)
        if (set.pBoni2[i].nProperty >= 0) sub_6FD95810(4, unit, item, nullptr, i, 0, 0, &set.pBoni2[i], state, 0, nullptr);
    if (count >= set.nSetItems)
        for (int i = 0; i < 8 && set.pFBoni[i].nProperty >= 0; ++i)
            sub_6FD95810(4, unit, item, nullptr, i, 0, 0, &set.pFBoni[i], state, 0, nullptr);
}

// 1.13c D2Common+0x45550
// Mandatory seams are the three retail callees, with their register operands explicit.
void D2Common_45550_RoomWarps(void* pool, D2DrlgRoomStrc* room,
    void (*nearRooms)(void*, D2DrlgRoomStrc*),
    void (*warp)(void*, D2DrlgRoomStrc*, uint8_t),
    void (*finish)(D2DrlgRoomStrc*))
{
    nearRooms(pool, room);
    for (uint32_t bit = 0x10, index = 0; bit & 0xff0; bit <<= 1, ++index)
        if ((room->dwFlags & bit) && room->pLevel->nLevelId != 133) warp(pool, room, static_cast<uint8_t>(index));
    finish(room);
}

// 1.13c D2Common+0x4A670
void D2Common_4A670_OutdoorGrids(D2DrlgRoomStrc* room)
{
    auto* outdoor = room->pOutdoor;
    auto* pool = room->pLevel->pDrlg->pMempool;
    const int width = room->nTileWidth + 1, height = room->nTileHeight + 1;
    auto* def = DATATBLS_GetLevelDefRecord(room->pLevel->nLevelId);
    DRLGGRID_InitializeGridCells(pool, &outdoor->pTileTypeGrid, width, height);
    DRLGGRID_InitializeGridCells(pool, &outdoor->pWallGrid, width, height);
    DRLGGRID_InitializeGridCells(pool, &outdoor->pFloorGrid, width, height);
    for (int y = 0; y < 8; ++y) for (int x = 0; x < 8; ++x)
        DRLGGRID_AlterGridFlag(&outdoor->pFloorGrid, x, y, 0x40002, FLAG_OPERATION_OVERWRITE);
    if (DRLG_GetActNoFromLevelId(room->pLevel->nLevelId) == 0) DRLG_OUTDOORS_GenerateDirtPath(room->pLevel, room);
    DRLGROOMTILE_AllocTileGrid(room);
    D2UnkOutdoorStrc2 theme = {};
    theme.pDrlgRoom = room;
    theme.pOutdoorRooms[0] = outdoor;
    theme.pWallsGrids[0] = &outdoor->pWallGrid;
    theme.pFloorGrid = &outdoor->pFloorGrid;
    theme.field_2C = 1;
    const uint32_t waypoint = room->dwFlags >> 16 & 3, shrine = room->dwFlags >> 12 & 15;
    if (waypoint) { theme.nSubWaypoint_Shrine = def->dwSubWaypoint; theme.nSubTheme = 0; theme.nSubThemePicked = waypoint; sub_6FD8AA80(&theme); }
    if (shrine) { theme.nSubWaypoint_Shrine = def->dwSubShrine; theme.nSubTheme = 0; theme.nSubThemePicked = shrine; sub_6FD8AA80(&theme); }
    theme.nSubWaypoint_Shrine = outdoor->nSubType;
    theme.nSubTheme = outdoor->nSubTheme;
    theme.nSubThemePicked = outdoor->nSubThemePicked;
    sub_6FD8AA80(&theme);
    uint32_t flags = 0;
    switch (room->pLevel->nLevelType) {
        case 16: flags = 0x100; break;
        case 21: flags = 0x120000; break;
        case 22: flags = 0x100000; break;
        case 27: flags = 0xa00000; break;
        case 28: flags = 0x1600000; break;
        case 31: flags = room->pLevel->nLevelId == LEVEL_TUNDRAWASTELANDS ? 0x600000 : 0; break;
    }
    for (int y = 0; y < height; ++y) for (int x = 0; x < width; ++x)
        if (!(DRLGGRID_GetGridEntry(&outdoor->pFloorGrid, x, y) & 0x3f0ff80)) DRLGGRID_AlterGridFlag(&outdoor->pFloorGrid, x, y, flags, FLAG_OPERATION_OR);
    DRLGGRID_AlterEdgeGridFlags(&outdoor->pWallGrid, 4, FLAG_OPERATION_OR);
    DRLGGRID_AlterEdgeGridFlags(&outdoor->pFloorGrid, 4, FLAG_OPERATION_OR);
}

// 1.13c D2Common+0x4A9B0
void D2Common_4A9B0_OutdoorTiles(D2DrlgRoomStrc* room)
{
    DRLGROOMTILE_AllocTileGrid(room);
    auto* outdoor = room->pOutdoor;
    DRLGROOMTILE_CountWallWarpTiles(room, &outdoor->pWallGrid, &outdoor->pTileTypeGrid, 0, 0);
    DRLGROOMTILE_CountAllTileTypes(room, &outdoor->pWallGrid, 0, 0, 0);
    DRLGROOMTILE_CountAllTileTypes(room, &outdoor->pFloorGrid, 0, 0, 0);
    DRLGROOMTILE_AllocTileData(room);
    DRLGROOMTILE_LoadInitRoomTiles(room, &outdoor->pWallGrid, &outdoor->pTileTypeGrid, 0, 0, 0);
    DRLGROOMTILE_LoadInitRoomTiles(room, &outdoor->pFloorGrid, nullptr, 0, 0, 0);
    room->pTileGrid->pTiles.nWalls = room->pTileGrid->nWalls;
    room->pTileGrid->pTiles.nFloors = room->pTileGrid->nFloors;
    room->pTileGrid->pTiles.nRoofs = room->pTileGrid->nShadows;
    DRLGLOGIC_AllocCoordLists(room);
}
