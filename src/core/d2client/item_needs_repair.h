#pragma once
// 1.13c D2Client+0x45B40: does an item need the smith. One rule for the server unit (the local session) and the
// packet client's own item unit, so the repair confirm reads the same test on either path.
#include <D2Items.h>
#include <D2StatList.h>

inline int d2client_item_needs_repair(D2UnitStrc* item) {
    if (!item) return 0;
    // Item type 4 and items ITEMS_IsRepairable refuses (unidentified, ethereal, no Repair flag) never need repair;
    // then durability below max, a stackable below its max stack, or a charged skill with used charges.
    if (ITEMS_GetItemType(item) == 4 || !ITEMS_IsRepairable(item)) return 0;
    if (ITEMS_HasDurability(item)) {
        const int32_t maxDur = STATLIST_GetMaxDurabilityFromUnit(item);
        if (maxDur && STATLIST_UnitGetStatValue(item, STAT_DURABILITY, 0) < maxDur) return 1;
    }
    if (ITEMS_CheckIfStackable(item) && STATLIST_UnitGetStatValue(item, STAT_QUANTITY, 0) < ITEMS_GetTotalMaxStack(item)) return 1;
    BOOL full = FALSE;
    return ITEMS_HasUsedCharges(item, &full) && !full ? 1 : 0;
}
