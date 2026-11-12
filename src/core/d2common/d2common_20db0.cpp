// 1.13c D2Common+0x20DB0: the hand-pair answer of the body-location placement check (0x211D0's
// 4/5 and 11/12 cases). Retail is a register call: EAX = the item already at the requested
// location, ECX = the item at the paired location, EDX = the item being placed, then the player
// and the paired location on the stack (ret 8).
//   no incoming item: 3 when the requested location is taken, 4 when the paired item is a
//     two-handed item (type 4 and the items.txt two-handed byte), else 0;
//   incoming, requested location free: 1 without a paired item, else 2 - (0x1F980 compatible);
//   incoming, requested location taken: 6 when the two stack (0x26930), 5 when 0x1F980 accepts
//     the pair, else 7 when the paired location's item fits the player's inventory page
//     (0x20B50 with the 0x2FB60 inventory record), else 0.
// The 1.10f twin is sub_6FD91D50 (D2Inventory.cpp); 1.13c calls 0x1F980 (d2_port_d2common_1f980)
// in place of the 1.10f ammo-pair test and tests the paired slot for the free-position branch.
#include <cstdint>

#include "D2Inventory.h"
#include "D2Items.h"
#include "Units/Units.h"

extern D2InventoryGridInfoStrc gBodyLocInventoryGridInfo;
extern "C" int32_t d2_port_d2common_1f980(D2UnitStrc* unit, D2UnitStrc* first, D2UnitStrc* second);

int32_t D2Common_20DB0_HandPairAnswer(D2UnitStrc* pOther, D2UnitStrc* pIncoming, D2UnitStrc* pPlayer,
	int32_t nOtherBodyLoc, D2UnitStrc* pAtLoc) // 1.13c D2Common+0x20DB0
{
	if (!pIncoming)
	{
		if (pAtLoc)
		{
			return 3;
		}

		if (pOther && pOther->dwUnitType == UNIT_ITEM && ITEMS_CheckWeaponIfTwoHandedByItemId(pOther->dwClassId))
		{
			return 4;
		}

		return 0;
	}

	if (!pAtLoc)
	{
		if (pOther)
		{
			return 2 - (d2_port_d2common_1f980(pPlayer, pIncoming, pOther) != 0);
		}

		return 1;
	}

	if (ITEMS_AreStackablesEqual(pAtLoc, pIncoming))
	{
		return 6;
	}

	if (d2_port_d2common_1f980(pPlayer, pIncoming, pOther))
	{
		return 5;
	}

	// 0x20B50: the paired slot's item, given an incoming item, must have a free position on page 0.
	const int32_t nInventoryRecord = UNITS_GetInventoryRecordId(pPlayer, 0, 1);
	D2InventoryStrc* pInventory = pPlayer->pInventory;
	if (!INVENTORY_GetPtrIfValid(pInventory) || nOtherBodyLoc < 0 || nOtherBodyLoc >= 13)
	{
		return 0;
	}

	D2InventoryGridStrc* pGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
	D2UnitStrc* pPaired = pGrid ? pGrid->ppItems[nOtherBodyLoc] : nullptr;
	if (!pPaired)
	{
		return 0;
	}

	int32_t nX = 0;
	int32_t nY = 0;
	return INVENTORY_GetFreePosition(pInventory, pPaired, nInventoryRecord, &nX, &nY, 0) ? 7 : 0;
}
