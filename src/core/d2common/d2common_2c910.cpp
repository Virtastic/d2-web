// 1.13c D2Common+0x2C910 (ordinal 10600, 1.10f ITEMS_UpdateSets): refresh a set item's set states
// and the owner's set bonus, routed through the 1.13c helpers 0x2C7C0 and 0x2C630 rather than the
// 1.10f sub_6FDA4380 / sub_6FDA4490 the vendored ITEMS_UpdateSets still calls.
#include <cstdint>

#include "D2Inventory.h"
#include "D2ItemMods.h"
#include "D2Items.h"

int32_t D2Common_2C630_SetItemApplyBonus(D2UnitStrc* pUnit, D2UnitStrc* pItem, int32_t nMode);
extern "C" int32_t D2Common_2C7C0_SetItemToggleStates(D2UnitStrc* pItem, uint32_t nSetItemMask);

// 1.13c D2Common+0x2C910
int32_t D2Common_2C910_UpdateSets(D2UnitStrc* pUnit, D2UnitStrc* pItem, int32_t nMode, int32_t bForceStates)
{
	if (pUnit == nullptr)
		return 0;
	if (pItem == nullptr || pItem->dwUnitType != UNIT_ITEM || pUnit->pInventory == nullptr)
		return 0;

	uint32_t nSetItemMask = 0;
	// 0x1E880 hands back the item when the inventory owns it; 0x1E8C0 reads its node page.
	if (INVENTORY_IsItemInInventory(pUnit->pInventory, pItem) != nullptr
		&& INVENTORY_GetItemNodePage(pItem) == 3)
		nSetItemMask = ITEMS_GetSetItemsMask(pUnit, pItem, TRUE); // 0x23F90

	if (bForceStates != 0 || nMode == 0)
		D2Common_2C7C0_SetItemToggleStates(const_cast<D2UnitStrc*>(pItem), nSetItemMask);

	D2Common_2C630_SetItemApplyBonus(pUnit, pItem, nMode);
	return 1;
}
