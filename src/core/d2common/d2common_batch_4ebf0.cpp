// Prior art used: D2MOO D2Skills.cpp (sub_6FDB1050/sub_6FDB1130),
// D2Inventory, D2Seed and native D2DrlgLevelStrc/D2DrlgRoomStrc fields.
// Retail 1.13c rearranges DRLG fields; byte offsets are not native layouts.
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include "D2Inventory.h"
#include "D2Items.h"
#include "D2Skills.h"
#include "D2DataTbls.h"
#include "D2StatList.h"
#include "D2Seed.h"
#include "Drlg/D2DrlgDrlg.h"
#include "Units/Units.h"
#include "Units/Item.h"
#include "DataTbls/MonsterTbls.h"

// 1.13c D2Common+0x4EBF0
extern "C" D2UnitStrc* d2_fn_d2common_4ebf0(D2InventoryStrc* inventory, int32_t bodyLoc)
{
    return INVENTORY_GetItemFromBodyLoc(inventory, bodyLoc == 5 ? 4 : 5);
}

static_assert(offsetof(D2SkillsTxt, nSkillId) == 0);
static_assert(offsetof(D2SkillsTxt, nItemTypeAB) == 0x18);
static_assert(offsetof(D2SkillsTxt, nElemTypeAB) == 0x24);
static_assert(offsetof(D2ItemTypesTxt, wShoots) == 0xc);

// 1.13c D2Common+0x50610
extern "C" bool d2_fn_d2common_50610(D2UnitStrc* item, D2UnitStrc* unit,
    D2SkillsTxt* skill, int32_t type)
{
    const auto* words = reinterpret_cast<const int16_t*>(skill);
    if (!item) {
        if (words[2 * type + 0x12] <= 0 && words[3 * type + 0xc] <= 0) return true;
        const int16_t include = words[0xc];
        if ((include == 45 || include == 46 || include == 67) && words[0xf] <= 0
            && !ITEMS_CheckItemTypeId(nullptr, 45) && !ITEMS_CheckItemTypeId(unit, 45)) return true;
        if (include == 67 && words[0xf] <= 0 && !ITEMS_CheckItemTypeId(nullptr, 45)
            && !ITEMS_CheckItemTypeId(unit, 45)) return true;
        return false;
    }
    const int16_t* excluded = words + 2 * type + 0x12;
    for (int32_t i = 0; i < 2 && excluded[i] > 0; ++i)
        if (ITEMS_CheckItemTypeId(item, excluded[i])) return false;
    const int16_t* included = words + 3 * type + 0xc;
    int32_t i = 0;
    for (;;) {
        if (included[i] <= 0) return i == 0;
        if (ITEMS_CheckItemTypeId(item, included[i])) break;
        if (++i == 3) return false;
    }
    if (item->dwUnitType == UNIT_ITEM) {
        if (item->pItemData && (item->pItemData->dwItemFlags & 0x4100)) return words[0] == 0;
        const int32_t itemType = ITEMS_GetItemType(item);
        D2ItemTypesTxt* record = DATATBLS_GetItemTypesTxtRecord(itemType);
        if (record && record->wShoots && !(reinterpret_cast<const uint8_t*>(skill)[6] & 1)
            && (words[0] || !item->pStatListEx || sgptDataTables->nItemStatCostTxtRecordCount < 158
                || !STATLIST_GetTotalStat_6FDB63E0(
                    reinterpret_cast<D2StatListStrc*>(item->pStatListEx),
                    D2SLayerStatIdStrc::Make(0, 157).nPackedValue,
                    &sgptDataTables->pItemStatCostTxt[157]))) {
            if (!unit || !ITEMS_CheckItemTypeId(unit, ITEMS_GetAmmoType(item))) return false;
            if ((words[0] || !STATLIST_UnitGetItemStatOrSkillStatValue(unit, 157, 0))
                && ITEMS_GetTotalMaxStack(unit) && STATLIST_UnitGetStatValue(unit, 70, 0) < 1) return false;
        }
    }
    return true;
}

// Retail level+0x08 is nRooms, +0x10 is pFirstRoomEx, +0x1c4 is pSeed;
// room+0x24 is pDrlgRoomNext. Use the native fields after pointer expansion.
// 1.13c D2Common+0x19A50
extern "C" D2DrlgRoomStrc* d2_fn_d2common_19a50(D2DrlgLevelStrc* level)
{
    if (!level || !level->pFirstRoomEx) std::abort();
    uint32_t index = SEED_RollLimitedRandomNumber(&level->pSeed, level->nRooms);
    auto* room = level->pFirstRoomEx;
    while (index--) room = room->pDrlgRoomNext;
    return room;
}

// The 0x19950 callee returns the eight warp IDs, resolving per-DRLG overrides.
using D2C113WarpIds = const int32_t* (*)(void*, int32_t);
// Retail level+0x1b4 is pDrlg, +0x1d0 is nLevelId; room+0x28 is dwFlags.
// 1.13c D2Common+0x19AD0
extern "C" D2DrlgRoomStrc* d2_fn_d2common_19ad0(D2DrlgLevelStrc* level,
    D2C113WarpIds warpIds)
{
    for (auto* room = level->pFirstRoomEx; room; room = room->pDrlgRoomNext) {
        if (!(room->dwFlags & 0xff0)) continue;
        for (uint32_t i = 0, mask = 0x10; mask & 0xff0; ++i, mask <<= 1)
            if ((room->dwFlags & mask) && warpIds(level->pDrlg, level->nLevelId)[i] != -1) return room;
    }
    return nullptr;
}
