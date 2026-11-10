// 1.13c D2Client+0xA0A30
// Whether a unit has ammunition for its hand weapon. A non-player always has. A player with no
// weapon has. A weapon that shoots something (its item type's Shoots) is satisfied by any hand
// item with a quantity; a plain Attack with the weapon's magic-arrow stat needs none. A weapon that
// shoots nothing must itself be the stackable or throwable hand item. The answer is that item's
// "has quantity left" test.
#include <cstdint>
#include <D2Inventory.h>
#include <D2Items.h>
#include <D2Skills.h>
#include <D2StatList.h>
#include <Units/Units.h>
#include "realm_skill_start.h"

// 1.13c D2Common #10104: quantity above 0; or the throwable stat, which also zeroes the quantity;
// or an item that does not stack at all.
static int item_has_quantity(D2UnitStrc* item) {
    if (!item) return 0;
    if (item->pStatListEx) {
        if (static_cast<int32_t>(STATLIST_UnitGetStatValue(item, STAT_QUANTITY, 0)) > 0) return 1;
        if (STATLIST_UnitGetItemStatOrSkillStatValue(item, STAT_ITEM_THROWABLE, 0)) {
            STATLIST_SetUnitStat(item, STAT_QUANTITY, 0, 0);
            return 1;
        }
    }
    return ITEMS_GetTotalMaxStack(item) < 1 ? 1 : 0;
}

int d2client_realm_a0a30(D2UnitStrc* unit) {
    if (!unit) return 0;
    if (unit->dwUnitType != UNIT_PLAYER) return 1;
    D2UnitStrc* weapon = D2Common_10434(unit, 0);
    if (!weapon) return 1;
    bool mustBeWeapon = true;
    if (ITEMS_GetAmmoType(weapon)) {
        mustBeWeapon = false;
        D2SkillStrc* used = UNITS_GetUsedSkill(unit);
        if (used && SKILLS_GetSkillIdFromSkill(used, __FILE__, __LINE__) == 0 &&
            STATLIST_UnitGetItemStatOrSkillStatValue(weapon, STAT_ITEM_MAGICARROW, 0)) return 1;
    }
    if (!unit->pInventory) return 0;
    for (int slot = BODYLOC_RARM; slot <= BODYLOC_LARM; ++slot) {
        D2UnitStrc* item = INVENTORY_GetItemFromBodyLoc(unit->pInventory, slot);
        if (!item || (mustBeWeapon && item != weapon)) continue;
        if (ITEMS_CheckIfStackable(item) || ITEMS_CheckIfThrowable(item) ||
            STATLIST_UnitGetItemStatOrSkillStatValue(item, STAT_ITEM_THROWABLE, 0))
            return item_has_quantity(item);
    }
    return 0;
}
