// src/core/d2common/d2common_batch_38f60.cpp -- seven 1.13c D2Common.dll rows transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2common/decomp/0x<RVA>.c). Image base 0x6FD50000.
//
// Each definition carries a trailing `// 1.13c D2Common+0x<rva>` marker on its own line. Callees
// that src/ does not expose as callable C++ (still-UNMAPPED rows reached through a protocol
// surface) are env-seam function pointers; a null seam contributes 0 and the body never faults.
// Fatal paths that end in _exit(-1) are modelled as returns, as in the other d2common batches.
#include <cstdint>
#include <emscripten.h>

#include "D2Inventory.h"
#include "D2Items.h"
#include "D2Skills.h"
#include "D2StatList.h"
#include "Drlg/D2DrlgDrlg.h"
#include "Units/Units.h"

// Defined in the vendored D2Inventory.cpp, not declared in a header.
extern D2InventoryGridInfoStrc gBodyLocInventoryGridInfo;

// Implemented in other d2common batches; declared here without a header.
D2UnitStrc* D2Common_20320_GetLeftHandWeapon(D2InventoryStrc* pInventory);
bool D2Common_22EC0_CheckWeaponClassByItemId(uint32_t nItemId, int32_t nWeapClass);
BOOL D2Common_10217_ITEMS_CheckBodyLocation(D2UnitStrc* pItem, uint32_t nBodyLoc);
// d2common_20db0.cpp: the hand-pair answer (retail EAX = the item at the requested location, named last here).
int32_t D2Common_20DB0_HandPairAnswer(D2UnitStrc* pOther, D2UnitStrc* pIncoming, D2UnitStrc* pPlayer,
	int32_t nOtherBodyLoc, D2UnitStrc* pAtLoc);

// 1.13c D2Common+0x32EF0 (#10932): merge the dual-wield weapon stat lists into the unit. The
// weapon 0x31B90 selects and the left-hand weapon decide which of the two hands is merged with
// type 1 and which with type 0; only a weapon whose stat list has an owning extended parent
// (STATLIST_GetOwner non-null) is merged.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_32EF0_MergeDualWield(uint32_t pUnit, int32_t bFlag) { // 1.13c D2Common+0x32EF0
	D2UnitStrc* pUn = reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pUnit));

	if (!UNITS_CanDualWield(pUn))
		return;

	D2UnitStrc* pWeapon = D2Common_10434(pUn, bFlag);
	D2UnitStrc* pLeft = D2Common_20320_GetLeftHandWeapon(pUn->pInventory);

	if (!pWeapon)
	{
		if (pLeft && STATLIST_GetOwner(pLeft, nullptr))
			STATLIST_MergeStatLists(pUn, pLeft, 1);
		return;
	}

	if (pWeapon == pLeft)
	{
		D2UnitStrc* pRight = INVENTORY_GetSecondaryWeapon(pUn->pInventory);

		if (pLeft && STATLIST_GetOwner(pLeft, nullptr))
			STATLIST_MergeStatLists(pUn, pLeft, 1);
		if (pRight && STATLIST_GetOwner(pRight, nullptr))
			STATLIST_MergeStatLists(pUn, pRight, 0);
		return;
	}

	if (STATLIST_GetOwner(pWeapon, nullptr))
		STATLIST_MergeStatLists(pUn, pWeapon, 1);
	if (pLeft && STATLIST_GetOwner(pLeft, nullptr))
		STATLIST_MergeStatLists(pUn, pLeft, 0);
}

// 1.13c D2Common+0x12450 (#10933, D2GetRoom): non-zero when the second unit's room list contains
// the first unit's room. 0x3BA20 supplies that pair from the room; the 1.13c room keeps them at
// +0x00 (list) and +0x24 (count), a layout delta from the vendored 1.10f D2ActiveRoomStrc, whose
// same two fields are read by name here. Null units are fatal 0x18/0x19 in retail, modelled as 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_batch_d2common_12450(uint32_t pUnit1, uint32_t pUnit2) { // 1.13c D2Common+0x12450
	D2UnitStrc* pUn1 = reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pUnit1));
	D2UnitStrc* pUn2 = reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pUnit2));

	if (!pUn1 || !pUn2)
		return 0;

	D2ActiveRoomStrc* pRoom2 = UNITS_GetRoom(pUn2);
	D2ActiveRoomStrc* pRoom1 = UNITS_GetRoom(pUn1);
	if (!pRoom2 || !pRoom1)
		return 0;

	for (int32_t i = 0; i < pRoom2->nNumRooms; ++i)
	{
		if (pRoom2->ppRoomList[i] == pRoom1)
			return 1;
	}
	return 0;
}


// 1.13c D2Common+0x203B0 (#10937): non-zero when a weapon of the requested weapon class sits in
// either hand. The left hand is checked with the raw broken/no-equip item-data flags of the
// decompile (ITEMS_CanBeEquipped is the same pair for a present item data), the right hand always
// through 0x23EF0. The grid is only read when the inventory carries signature 0x01020304.
int32_t D2Common_203B0_HasWeaponClassEquipped(uint32_t pInventory, int32_t nWeapClass) { // 1.13c D2Common+0x203B0
	D2InventoryStrc* pInv = reinterpret_cast<D2InventoryStrc*>(static_cast<uintptr_t>(pInventory));

	if (!pInv)
		return 0;

	D2UnitStrc* pItem = nullptr;
	if (INVENTORY_CheckSignature(pInv))
	{
		D2InventoryGridStrc* pGrid = INVENTORY_GetGrid(pInv, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
		if (pGrid)
		{
			pItem = pGrid->ppItems[BODYLOC_LARM];
			if (pItem && pItem->dwUnitType == UNIT_ITEM && ITEMS_CanBeEquipped(pItem) &&
				D2Common_22EC0_CheckWeaponClassByItemId(pItem->dwClassId, nWeapClass))
			{
				return 1;
			}

			pItem = pGrid->ppItems[BODYLOC_RARM];
		}
	}

	if (pItem && pItem->dwUnitType == UNIT_ITEM && ITEMS_CanBeEquipped(pItem) &&
		D2Common_22EC0_CheckWeaponClassByItemId(pItem->dwClassId, nWeapClass))
	{
		return 1;
	}
	return 0;
}

// 1.13c D2Common+0x211D0 (#10946): the body-location placement answer for an item. With an item,
// 0x24C80 must accept the location and, unless the requirement check is skipped, 0x26DB0 must
// pass. Body location 0 answers 0; the hand pairs (4/5 and 11/12) answer through 0x20DB0 with the
// item at the paired location and the item already at the requested one; every other location
// answers 5/1 with an item present or 3/0 without one. Retail is thiscall; ECX only passes through
// to 0x24C80 and is not read. The 0x26DB0 requirement check is ITEMS_CheckRequirements.
uint32_t D2Common_211D0_BodyLocAnswer(D2UnitStrc* pUn, uint32_t nBodyLoc, D2UnitStrc* pInvItem, int32_t bDontCheckReqs) { // 1.13c D2Common+0x211D0
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

// 1.13c D2Common+0x304A0 (#10947): the skill list's first skill (list+0x04), or 0 without a
// skill list. A null unit is fatal 0x638 in retail, modelled as 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Common_304A0_FirstSkill(uint32_t pUnit) { // 1.13c D2Common+0x304A0
	if (!pUnit)
		return 0;
	const uint32_t pSkills = *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(pUnit) + 0xA8);
	if (!pSkills)
		return 0;
	return *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(pSkills) + 4);
}
