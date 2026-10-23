// Stage A: 1.13c entry points recovered from the cached D2Common decompiles.
// Prior art used: D2MOO collision, item, environment, skill and inventory algorithms;
// (D2MOO is the prose map; 1.13c controls arithmetic and defaults).
// Native pointers below address the wasm64 game objects. Retail table fields retain
// their packed sizes; unit/player/environment members are accessed by name.
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include "D2Collision.h"
#include "D2DataTbls.h"
#include "D2Environment.h"
#include "D2Inventory.h"
#include "D2Items.h"
#include "D2Skills.h"
#include "D2StatList.h"
#include "Units/Units.h"
#include "Units/Player.h"

static_assert(offsetof(D2ItemTypesTxt, nReEquip) == 0x12);
static_assert(offsetof(D2ItemsTxt, dwBitField1) == 0xdc);
static_assert(offsetof(D2SkillsTxt, dwFlags) == 4);
static_assert(offsetof(D2SkillsTxt, nReqSkill) == 0x17e);

// 1.13c D2Common+0x4d810
extern "C" void D2Common_4D810_FindFreeCoordinates(D2ActiveRoomStrc* room,
    D2CoordStrc* position, int size, uint32_t mask, int increment)
{
    COLLISION_GetFreeCoordinatesEx(room, position, size, mask, increment);
}

// 1.13c D2Common+0x234f0
extern "C" uint8_t D2Common_234F0_GetReEquip(D2UnitStrc* item)
{
    if (!item || item->dwUnitType != UNIT_ITEM) return 0;
    const int type = static_cast<int>(ITEMS_GetItemType(item));
    if (type < 0 || type >= sgptDataTables->nItemTypesTxtRecordCount) return 0;
    return sgptDataTables->pItemTypesTxt[type].nReEquip;
}

// 1.13c D2Common+0x373B0
extern "C" void D2Common_373B0_GetEnvironmentState(D2DrlgActStrc* act,
    int* cycle, int* ticks, BOOL* eclipse)
{
    ENVIRONMENT_GetCycleIndex_Ticks_EclipseFromAct(act, cycle, ticks, eclipse);
}

// 1.13c D2Common+0x22760
extern "C" uint32_t D2Common_22760_IsMetal(uint32_t itemId)
{
    // The table helper preserves retail's fatal invalid-index contract.
    return ITEMS_IsMetalItem(static_cast<int32_t>(itemId));
}

// 1.13c D2Common+0x4E5A0
extern "C" uint32_t D2Common_4E5A0_GetSkillFlag(int skillId)
{
    const auto* row = DATATBLS_GetSkillsTxtRecord(skillId);
    return row ? (row->dwFlags[0] >> 16) & 2u : 0;
}

// 1.13c D2Common+0x2F520
extern "C" void D2Common_2F520_SetSwitchRightSkill(D2UnitStrc* unit, int id, int flags)
{
    UNITS_SetSwitchRightSkill(unit, id, flags);
}

// 1.13c D2Common+0x37840
extern "C" void D2Common_37840_NextTimeRate(D2DrlgActStrc* act, D2ActiveRoomStrc* room)
{
    ENVIRONMENT_SetNextTimeRate(act, room);
}

namespace {
int UnitLevel(D2UnitStrc* unit)
{
    if (!unit || !unit->pStatListEx || sgptDataTables->nItemStatCostTxtRecordCount < 13)
        return 0;
    return STATLIST_UnitGetStatValue(unit, STAT_LEVEL, 0);
}
int OwnedSkillLevel(D2UnitStrc* unit, int id)
{
    if (!unit || !unit->pSkills) return 0;
    auto* skill = SKILLS_GetSkill(unit, id, -1);
    return skill ? skill->nSkillLevel : 0;
}
}

// 1.13c D2Common+0x2CC60
extern "C" uint32_t D2Common_2CC60_HirelingReviveCost(D2UnitStrc* unit)
{
    const uint32_t level = static_cast<uint32_t>(UnitLevel(unit));
    // Unlike the 1.10f map, retail divides the wrapped square BEFORE multiplying by 15.
    const int32_t square = static_cast<int32_t>(level * level);
    const uint32_t cost = static_cast<uint32_t>(square / 2) * 15u;
    return cost > 50000u ? 50000u : cost;
}

// 1.13c D2Common+0x4FB30
extern "C" BOOL D2Common_4FB30_CheckRequiredSkills(D2UnitStrc* unit, int skillId)
{
    auto* row = DATATBLS_GetSkillsTxtRecord(skillId);
    if (!row) return FALSE;
    const int level = UnitLevel(unit);
    const int requiredLevel = row->wReqLevel + OwnedSkillLevel(unit, skillId);
    if (requiredLevel > level) return FALSE;
    for (const int required : row->nReqSkill) {
        if (required < 0) continue;
        auto* requiredRow = DATATBLS_GetSkillsTxtRecord(required);
        if (!requiredRow) return FALSE;
        bool found = false;
        if (unit && unit->pSkills) {
            for (auto* skill = unit->pSkills->pFirstSkill; skill; skill = skill->pNextSkill)
                if (skill->pSkillsTxt == requiredRow) { found = true; break; }
        }
        if (!found || OwnedSkillLevel(unit, required) < 1) return FALSE;
    }
    return TRUE;
}

// 1.13c D2Common+0x26930
extern "C" BOOL D2Common_26930_AreStackablesEqual(D2UnitStrc* first, D2UnitStrc* second)
{
    return ITEMS_AreStackablesEqual(first, second);
}

// 1.13c D2Common+0x303E0
extern "C" void D2Common_303E0_SetUsedSkill(D2UnitStrc* unit, D2SkillStrc* skill)
{
    if (!unit) std::abort();
    if (unit->pSkills) unit->pSkills->pUsedSkill = skill;
}

// 1.13c D2Common+0x21dd0
extern "C" void D2Common_21DD0_PlaceAtPage(D2InventoryStrc* inventory,
    D2UnitStrc* item, int x, int y, int record, BOOL unused, uint8_t page)
{
    if (inventory) INVENTORY_PlaceItemInGrid(inventory, item, x, y, page + 2, record, unused);
}
