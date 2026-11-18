// Moved from the stand-alone d2common batch into the game tree.
#include <cstdint>

#include "D2Inventory.h"
#include "D2Items.h"
#include "Units/Units.h"

BOOL D2Common_10217_ITEMS_CheckBodyLocation(D2UnitStrc* pItem, uint32_t nBodyLoc);
// D2Inventory.cpp: the hand-pair answer (retail EAX = the item at the requested location, named last here).
int32_t D2Common_20DB0_HandPairAnswer(D2UnitStrc* pOther, D2UnitStrc* pIncoming, D2UnitStrc* pPlayer,
	int32_t nOtherBodyLoc, D2UnitStrc* pAtLoc);

// 1.13c D2Common+0x211D0 (#10946): the body-location placement answer for an item. With an item,
// 0x24C80 must accept the location and, unless the requirement check is skipped, 0x26DB0 must
// pass. Body location 0 answers 0; the hand pairs (4/5 and 11/12) answer through 0x20DB0 with the
// item at the paired location and the item already at the requested one; every other location
// answers 5/1 with an item present or 3/0 without one. Retail is thiscall; ECX only passes through
// to 0x24C80 and is not read. The 0x26DB0 requirement check is ITEMS_CheckRequirements.
uint32_t D2Common_211D0_BodyLocAnswer(D2UnitStrc* pUn, uint32_t nBodyLoc, D2UnitStrc* pInvItem, int32_t bDontCheckReqs) {
	if (!pUn)
		return 0;

	D2InventoryStrc* pInventory = pUn->pInventory;
	if (!pInventory)
		return 0;

	if (pInvItem)
	{
		if (!D2Common_10217_ITEMS_CheckBodyLocation(pInvItem, nBodyLoc))
			return 0;
		if (!bDontCheckReqs && !ITEMS_CheckRequirements(pInvItem, pUn, TRUE, nullptr, nullptr, nullptr))
			return 0;
	}

	switch (nBodyLoc)
	{
	case BODYLOC_NONE:
		return 0;

	case BODYLOC_RARM:
	case BODYLOC_LARM:
	case BODYLOC_SWRARM:
	case BODYLOC_SWLARM:
	{
		const int nOtherBodyLoc = (nBodyLoc == BODYLOC_LARM) ? BODYLOC_RARM
			: (nBodyLoc == BODYLOC_SWLARM) ? BODYLOC_SWRARM
			: (nBodyLoc == BODYLOC_RARM) ? BODYLOC_LARM : BODYLOC_SWLARM;
		D2UnitStrc* pItemAtLoc = INVENTORY_GetItemFromBodyLoc(pInventory, static_cast<int>(nBodyLoc));
		D2UnitStrc* pItemOther = INVENTORY_GetItemFromBodyLoc(pInventory, nOtherBodyLoc);

		return static_cast<uint32_t>(D2Common_20DB0_HandPairAnswer(pItemOther, pInvItem, pUn, nOtherBodyLoc, pItemAtLoc));
	}

	default:
	{
		D2UnitStrc* pItemAtLoc = INVENTORY_GetItemFromBodyLoc(pInventory, static_cast<int>(nBodyLoc));
		if (pInvItem)
			return pItemAtLoc ? 5 : 1;
		return pItemAtLoc ? 3 : 0;
	}
	}
}
