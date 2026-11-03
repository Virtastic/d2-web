// 1.13c D2Common+0x2C7C0 (SetItemToggleStates): EBX = pItem, stack = nSetItemMask (ret n, regs ebx).
// Only a set-quality (5) item with a set record and a nonzero nAddFunc does anything. nAddFunc 1 sets or clears each
// state except the item's own set rank; nAddFunc 2 clears the states below the bonus count, then re-posts the rest
// of the owner's state lists (STATLIST_SET 0x2000) unless that flag is already present. Returns 1 once it ran, else 0.
// Ported from the vendored 1.10f sub_6FDA4380 body in src/core/d2common/d2common_batch_87080.cpp, moved here unchanged.
#include <cstdint>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Items.h"
#include "D2StatList.h"
#include "Units/Units.h"

// The set-state table and the set-item-count to bonus-count table, shared with the other set routines.
extern int gnItemSetStates[6];
extern int gnSetMaskToBonusMappingTable[64];

extern "C" EMSCRIPTEN_KEEPALIVE int32_t D2Common_2C7C0_SetItemToggleStates(D2UnitStrc* pItemS, uint32_t nSetItemMask)
{
	if (pItemS == nullptr)
		return 0;

	if (pItemS->dwUnitType != 4 || ITEMS_GetItemQuality(pItemS) != 5)
		return 0;

	D2SetItemsTxt* pSetItemsTxt = ITEMS_GetSetItemsTxtRecordFromItem(pItemS);
	if (pSetItemsTxt == nullptr || pSetItemsTxt->nAddFunc == 0)
		return 0;

	if (pSetItemsTxt->nAddFunc == 1)
	{
		const int32_t nSetItems = ITEMS_GetNoOfSetItemsFromItem(pItemS);
		for (int32_t i = 0; i < 6; ++i)
		{
			if (i == nSetItems)
				continue;

			int32_t nIndex = i;
			if (nSetItems < i)
				--nIndex;

			const bool bUnset = (nSetItemMask & (1u << i)) == 0;
			D2Common_10574(pItemS, gnItemSetStates[nIndex], bUnset ? 1 : 0);
		}
	}
	else if (pSetItemsTxt->nAddFunc == 2)
	{
		const int32_t nBonuses = nSetItemMask < 64
			? gnSetMaskToBonusMappingTable[nSetItemMask] : 0;
		const int32_t nFirst = nBonuses - 1;

		for (int32_t i = 0; i < nFirst; ++i)
			D2Common_10574(pItemS, gnItemSetStates[i], 0);

		if (nFirst >= 5)
			return 1;

		for (int32_t i = nFirst; i < 5; ++i)
		{
			// With zero bonuses retail starts one word before the table; the port reads that slot as zero.
			const int32_t nState = i < 0 ? 0 : gnItemSetStates[i];
			D2StatListExStrc* pStatListEx = pItemS->pStatListEx;
			if (nState == 0 || pStatListEx == nullptr || static_cast<int32_t>(pStatListEx->dwFlags) >= 0)
				continue;

			D2StatListStrc* pStateList = D2Common_GetStateFromStatListEx_6FDB8190(pStatListEx, nState);
			if (pStateList == nullptr || (pStateList->dwFlags & 0x2000u) != 0)
				continue;

			D2Common_ExpireStatList_6FDB6E30(pStateList);
			pStateList->dwFlags |= 0x2000u;
			D2COMMON_10475_PostStatToStatList(pItemS, pStateList, 1);
		}
	}

	return 1;
}
