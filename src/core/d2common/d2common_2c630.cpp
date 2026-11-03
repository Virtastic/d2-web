// 1.13c D2Common+0x2C630 (1.10f sub_6FDA4490): apply or remove one set item's bonus on its owner.
// EAX = the owner unit, stack: the item, nMode (0 add, 1 remove, >= 2 refused). The item must be a
// set item (quality 5); an owner that is itself an item must not be magic/set/rare/unique/crafted/
// tempered. The item's set-items row names the set id; the six item-set state lists (states at
// 0x6FDDCA08) are searched for the one whose STAT_VALUE holds that id, remembering the first empty
// state. Add into an empty state allocates a list (owner pool, the item's type and id), posts it to
// the owner and stamps the state; add then writes the set id and runs 0x45F90 with the state;
// remove expires and frees the list.
#include <cstdint>

#include "D2DataTbls.h"
#include "D2Items.h"
#include "D2StatList.h"
#include "DataTbls/ItemsTbls.h"
#include "Units/Units.h"

static_assert(offsetof(D2SetItemsTxt, nSetId) == 0x2C, "setitems.txt nSetId");

extern int gnItemSetStates[6];                                              // 0x6FDDCA08
D2SetItemsTxt* D2Common_1800_GetSetItemsTxtRecord(int32_t nSetItemId);      // 0x1800
void D2Common_3A330_AddStatLayer(D2StatListStrc* pStatList, int32_t nStatId, int32_t nDelta, uint16_t nLayer);   // 0x3A330
void D2Common_45F90_SetBonuses(D2UnitStrc* unit, D2UnitStrc* item, int state);

// 1.13c D2Common+0x2C630
int32_t D2Common_2C630_SetItemApplyBonus(D2UnitStrc* pUnit, D2UnitStrc* pItem, int32_t nMode)
{
	if (nMode >= 2 || pUnit == nullptr || pItem == nullptr)
		return 0;
	if (pItem->dwUnitType != UNIT_ITEM || ITEMS_GetItemQuality(pItem) != ITEMQUAL_SET)
		return 0;
	if (pUnit->dwUnitType == UNIT_ITEM && ITEMS_IsMagSetRarUniCrfOrTmp(pUnit) != 0)
		return 0;

	D2SetItemsTxt* pSetItemsTxt = D2Common_1800_GetSetItemsTxtRecord(ITEMS_GetFileIndex(pItem));
	if (pSetItemsTxt == nullptr)
		return 0;
	const int32_t nSetId = pSetItemsTxt->nSetId;
	if (nSetId < 0 || nSetId >= sgptDataTables->nSetsTxtRecordCount)
		return 0;

	int32_t nEmptyState = -1;
	int32_t nState = 0;
	D2StatListStrc* pStatList = nullptr;
	bool bFound = false;
	for (int32_t i = 0; i < 6; ++i)
	{
		nState = gnItemSetStates[i];
		pStatList = STATLIST_GetStatListFromUnitAndState(pUnit, nState);
		if (pStatList == nullptr)
		{
			if (nEmptyState < 0)
				nEmptyState = i;
			continue;
		}
		if (nSetId == STATLIST_GetStatValue(pStatList, STAT_VALUE, 0))
		{
			bFound = true;
			if (nMode == 0)
				STATLIST_RemoveAllStats(pStatList);
			break;
		}
	}

	if (!bFound)
	{
		if (nMode == 1 || nEmptyState < 0)
			return 0;
		nState = gnItemSetStates[nEmptyState];
		pStatList = STATLIST_AllocStatList(pUnit->pMemoryPool, 0, 0, static_cast<int>(pItem->dwUnitType), pItem->dwUnitId);
		D2COMMON_10475_PostStatToStatList(pUnit, pStatList, 1);
		if (pStatList != nullptr)
			STATLIST_SetState(pStatList, nState);
	}

	if (nMode == 0)
	{
		D2Common_3A330_AddStatLayer(pStatList, STAT_VALUE, nSetId, 0);
		D2Common_45F90_SetBonuses(pUnit, pItem, nState);
	}
	else
	{
		D2Common_ExpireStatList_6FDB6E30(pStatList);
		STATLIST_FreeStatList(pStatList);
	}
	return 1;
}
