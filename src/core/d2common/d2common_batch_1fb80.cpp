// D2Common implementation batch: four rows translated from the pinned 1.13c decompiles.
// The vendored D2MOO (1.10f) helpers are called by name; every control-flow decision, table
// bound and struct offset below is the 1.13c one, checked against the image disassembly.
#include <cstdint>

#include "D2DataTbls.h"
#include "D2Inventory.h"
#include "D2Items.h"
#include "D2Skills.h"
#include "D2StatList.h"
#include "D2States.h"
#include "DataTbls/SkillsTbls.h"
#include "Units/Units.h"

// The body-location grid info {13, 1}; defined in the vendored D2Inventory.cpp, not in a header.
extern D2InventoryGridInfoStrc gBodyLocInventoryGridInfo;

namespace
{
	// 1.13c D2Common+0x522C0: the state's stat list when it exists. With nSkillLevel 0 the list
	// is expired and freed; with a level it is allocated, given the state and posted to the unit.
	D2StatListStrc* GetOrCreateStateStatList(D2UnitStrc* pUnit, int nPassiveState, int nSkillLevel)
	{
		D2StatListStrc* pStatList = STATLIST_GetStatListFromUnitAndState(pUnit, nPassiveState);

		if (nSkillLevel == 0)
		{
			if (pStatList)
			{
				D2Common_10474(pUnit, pStatList);
				STATLIST_FreeStatList(pStatList);
			}

			return nullptr;
		}

		if (!pStatList)
		{
			pStatList = STATLIST_AllocStatList(pUnit->pMemoryPool, 0, 0, pUnit->dwUnitType, pUnit->dwUnitId);
			STATLIST_SetState(pStatList, nPassiveState);
			D2COMMON_10475_PostStatToStatList(pUnit, pStatList, 1);
		}

		return pStatList;
	}
}

// 1.13c D2Common+0x1FB80: wield type from the body-location grid's both hands (-1 none, 1 one or
// two one-handed/1-or-2-handed weapons, 2 a two-handed weapon). The grid is only read when the
// inventory carries signature 0x01020304; the player is only used by ITEMS_Is1Or2Handed.
int32_t D2Common_1FB80_GetWieldType(D2UnitStrc* pPlayer, D2InventoryStrc* pInventory)
{
	if (!pInventory)
	{
		return -1;
	}

	D2UnitStrc* pLeftHandItem = nullptr;
	D2UnitStrc* pRightHandItem = nullptr;

	if (INVENTORY_CheckSignature(pInventory))
	{
		D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
		if (pInventoryGrid)
		{
			pLeftHandItem = pInventoryGrid->ppItems[BODYLOC_LARM];
			pRightHandItem = pInventoryGrid->ppItems[BODYLOC_RARM];
		}
	}

	if (!pLeftHandItem)
	{
		if (!pRightHandItem)
		{
			return -1;
		}

		if (ITEMS_CheckWeaponIfTwoHanded(pRightHandItem))
		{
			return 2;
		}

		return ITEMS_Is1Or2Handed(pPlayer, pRightHandItem) ? 2 : -1;
	}

	if (!pRightHandItem)
	{
		if (ITEMS_CheckWeaponIfTwoHanded(pLeftHandItem))
		{
			return 2;
		}

		return ITEMS_Is1Or2Handed(pPlayer, pLeftHandItem) ? 2 : -1;
	}

	if (ITEMS_Is1Or2Handed(pPlayer, pLeftHandItem) || ITEMS_Is1Or2Handed(pPlayer, pRightHandItem))
	{
		return 1;
	}

	if (ITEMS_CheckWeaponIfTwoHanded(pLeftHandItem))
	{
		return 2;
	}

	if (ITEMS_CheckWeaponIfTwoHanded(pRightHandItem))
	{
		return 2;
	}

	return -1;
}

// 1.13c D2Common+0x1EDF0: first equipped item of item code nItemCode whose grid position is below
// 11. A non-null pCheckItem starts the walk at that item, which must be on the grid chain; any
// non-item, missing item data or a position of 11 and above ends the walk with no result.
D2UnitStrc* D2Common_1EDF0_GetEquippedItemByCode(D2InventoryStrc* pInventory, int nItemCode, D2UnitStrc* pCheckItem)
{
	if (!INVENTORY_GetPtrIfValid(pInventory))
	{
		return nullptr;
	}

	int nClassId = 0;
	if (!DATATBLS_GetItemRecordFromItemCode(nItemCode, &nClassId))
	{
		return nullptr;
	}

	D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
	if (!pInventoryGrid)
	{
		return nullptr;
	}

	D2UnitStrc* pItem = pInventoryGrid->pItem;

	if (pCheckItem)
	{
		while (pItem && pItem != pCheckItem)
		{
			pItem = INVENTORY_GetNextGridItem(pItem);
		}
	}

	while (pItem)
	{
		if (pItem->dwClassId == nClassId && UNITS_GetXPosition(pItem) < 11)
		{
			return pItem;
		}

		pItem = INVENTORY_GetNextGridItem(pItem);
	}

	return nullptr;
}

// 1.13c D2Common+0x20320: the left-hand weapon, found by the inventory's left-item GUID in either
// hand slot; the item must be a weapon (item type 0x2D) and carry that GUID.
D2UnitStrc* D2Common_20320_GetLeftHandWeapon(D2InventoryStrc* pInventory)
{
	if (!INVENTORY_GetPtrIfValid(pInventory) || pInventory->dwLeftItemGUID == D2UnitInvalidGUID)
	{
		return nullptr;
	}

	D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
	if (!pInventoryGrid)
	{
		return nullptr;
	}

	D2UnitStrc* pItem = pInventoryGrid->ppItems[BODYLOC_LARM];
	if (pItem && ITEMS_CheckItemTypeId(pItem, ITEMTYPE_WEAPON) && pItem->dwUnitId == pInventory->dwLeftItemGUID)
	{
		return pItem;
	}

	pItem = pInventoryGrid->ppItems[BODYLOC_RARM];
	if (pItem && ITEMS_CheckItemTypeId(pItem, ITEMTYPE_WEAPON) && pItem->dwUnitId == pInventory->dwLeftItemGUID)
	{
		return pItem;
	}

	return nullptr;
}

// 1.13c D2Common+0x52480: refresh a passive skill's state stat list. The skill must have a
// passive state below the states table count; with no skill, or while its aura state is active,
// the state list is torn down. Otherwise the level is recomputed, the state list is created if
// absent, and when its level stat differs from the current level the five passive stats are
// evaluated and written (layer = the passive item type when positive), then the skill and level
// stats are set and the state's graphics flag is raised.
void D2Common_52480_RefreshSkill(D2UnitStrc* pUnit, int nSkillId)
{
	if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount)
	{
		return;
	}

	D2SkillsTxt* pSkillsTxt = &sgptDataTables->pSkillsTxt[nSkillId];
	if (!pSkillsTxt)
	{
		return;
	}

	const int nPassiveState = pSkillsTxt->nPassiveState;
	if (nPassiveState <= 0 || nPassiveState >= sgptDataTables->nStatesTxtRecordCount)
	{
		return;
	}

	D2SkillStrc* pSkill = SKILLS_GetHighestLevelSkillFromSkillId(pUnit, nSkillId);
	if (!pSkill || (pSkillsTxt->nAuraState > 0 && STATES_CheckState(pUnit, pSkillsTxt->nAuraState)))
	{
		GetOrCreateStateStatList(pUnit, nPassiveState, 0);
		return;
	}

	const int nSkillLevel = SKILLS_GetSkillLevel(pUnit, pSkill, TRUE);

	D2StatListStrc* pStatList = GetOrCreateStateStatList(pUnit, nPassiveState, nSkillLevel);
	if (!pStatList || STATLIST_GetStatValue(pStatList, STAT_MODIFIERLIST_LEVEL, 0) == nSkillLevel)
	{
		return;
	}

	for (int i = 0; i < 5; ++i)
	{
		const int16_t nPassiveStat = pSkillsTxt->nPassiveStat[i];
		if (nPassiveStat < 0 || nPassiveStat >= sgptDataTables->nItemStatCostTxtRecordCount)
		{
			break;
		}

		const int nValue = SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwPassiveCalc[i], nSkillId, nSkillLevel);
		const uint16_t nLayer = pSkillsTxt->nPassiveIType > 0 ? (uint16_t)pSkillsTxt->nPassiveIType : 0;

		STATLIST_SetStat(pStatList, nPassiveStat, nValue, nLayer);
	}

	STATLIST_SetStat(pStatList, STAT_MODIFIERLIST_SKILL, nSkillId, 0);
	STATLIST_SetStat(pStatList, STAT_MODIFIERLIST_LEVEL, nSkillLevel, 0);
	STATES_ToggleGfxStateFlag(pUnit, nPassiveState, TRUE);
}
