// Retail 1.13c item wrappers cross-checked against the cached decompiles.
// Pointer fields use native D2MOO structs; scalar IDs retain retail widths.
#include <cstdint>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Items.h"
#include "D2StatList.h"
#include "Units/Units.h"
#include "Units/UnitRoom.h"
#include "D2States.h"

// 1.13c D2Common+0x22920 (#10211): ItemsTxt row `nItemId`, byte at +0x137 (nHasInv), or 0 when the
// id is out of range. Retail reads the loader's contiguous table directly; the bounds check is the
// same unsigned compare DATATBLS_GetItemsTxtRecord (1.13c+0x719A0) performs.
extern "C" EMSCRIPTEN_KEEPALIVE int D2Common_10211_ITEMS_CheckIfSocketableByItemId(int nItemId)
{
	const D2ItemsTxt* pItemsTxt = DATATBLS_GetItemsTxtRecord(nItemId);
	return pItemsTxt ? pItemsTxt->nHasInv : 0;
}

// 1.13c D2Common+0x229A0 (#10213): ITEMS_GetStrengthBonus. Null unit or non-item type (unit+0x00)
// yields 100; otherwise ItemsTxt[class id].nStrBonus (+0x106), unless that short is 1, in which
// case 100.
EMSCRIPTEN_KEEPALIVE short D2Common_10213_ITEMS_GetStrengthBonus(D2UnitStrc* pItem)
{
	return ITEMS_GetStrengthBonus(pItem);
}

// 1.13c D2Common+0x24C80 (#10217): ITEMS_CheckBodyLocation. The item's ItemTypesTxt body
// locations (ItemType index from ItemsTxt.wType[0], bytes +0x0A/+0x0B) match the requested one, or
// the request is 0xB/0xC (either left arm) and one of them is 4/5 (right/left arm).
EMSCRIPTEN_KEEPALIVE BOOL D2Common_10217_ITEMS_CheckBodyLocation(D2UnitStrc* pItem, uint32_t nBodyLoc)
{
	return nBodyLoc <= 0xFFu ? ITEMS_CheckBodyLocation(pItem, uint8_t(nBodyLoc)) : FALSE;
}

// 1.13c D2Common+0x23F40 (#10218): ITEMS_GetSetItemsTxtRecordFromItem. Only for an item
// (unit+0x00 == 4) with item data (unit+0x14) whose quality (+0x00) is set (5); returns
// SetItemsTxt[pItemData->dwFileIndex (+0x28)] when the index is in range, else null.
EMSCRIPTEN_KEEPALIVE D2SetItemsTxt* D2Common_10218_ITEMS_GetSetItemsTxtRecordFromItem(D2UnitStrc* pItem)
{
	return ITEMS_GetSetItemsTxtRecordFromItem(pItem);
}

// 1.13c D2Common+0x33020 (#10210): with pOwner, forwards to the owner-identity writer (+0x32CA0)
// using pOwner[0] and pOwner[3]. Otherwise, for a unit: clear identity words +0x94/+0x98; if it has
// a stat list (+0x5C) and the States table has more than 0x62 rows, toggle state 0x62 off and
// refresh the room (+0x665C0); if the list is an extended one (flags +0x10 < 0) find the state-0x62
// list in the two child chains (+0x3C, +0x40, next +0x2C, state +0x14), expire and free it. Always
// clears unit flag 0x400 (+0xC8).
// 1.13c D2Common+0x32CA0: owner-identity writer (unit in EDI, type/id on the stack).
static void SetOwnerSourceUnit(D2UnitStrc* pUnit, int32_t nType, int32_t nId)
{
	if (!pUnit) return;
	pUnit->dwOwnerType = nType;
	pUnit->dwOwnerGUID = nId;
	if (pUnit->pStatListEx) {
		if (sgptDataTables->nStatesTxtRecordCount > 0x62) {
			STATES_ToggleState(pUnit, 0x62, TRUE);
			UNITROOM_RefreshUnit(pUnit);
		}
		D2StatListStrc* pFound = nullptr;
		D2StatListExStrc* pEx = pUnit->pStatListEx;
		if (pEx && (pEx->dwFlags & 0x80000000u)) {
			for (D2StatListStrc* c = pEx->pMyLastList; c && !pFound; c = c->pPrevLink)
				if (c->dwStateNo == 0x62) pFound = c;
			for (D2StatListStrc* c = pEx->pMyStats; c && !pFound; c = c->pPrevLink)
				if (c->dwStateNo == 0x62) pFound = c;
		}
		if (!pFound) {
			pFound = STATLIST_AllocStatList(pUnit->pMemoryPool, 0, 0, pUnit->dwUnitType, pUnit->dwUnitId);
			if (!pFound) return;
			pFound->dwStateNo = 0x62;
			D2COMMON_10475_PostStatToStatList(pUnit, pFound, TRUE);
		}
		STATLIST_SetStat(pFound, 0x161, nType, 0);
		STATLIST_SetStat(pFound, 0x162, nId, 0);
	}
	pUnit->dwFlagEx |= 0x400u;
}

extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_10210_sub_33020(D2UnitStrc* pUnit, int32_t* pOwner)
{
	if (pOwner) { SetOwnerSourceUnit(pUnit, pOwner[0], pOwner[3]); return; }
	if (!pUnit) return;
	pUnit->dwOwnerType = 0;
	pUnit->dwOwnerGUID = 0;
	D2StatListExStrc* pList = pUnit->pStatListEx;
	if (pList) {
		if (sgptDataTables->nStatesTxtRecordCount > 0x62) {
			STATES_ToggleState(pUnit, 0x62, FALSE);
			UNITROOM_RefreshUnit(pUnit);
		}
		pList = pUnit->pStatListEx;
		if (pList && (pList->dwFlags & 0x80000000u)) {
			D2StatListStrc* pFound = nullptr;
			for (D2StatListStrc* c = pList->pMyLastList; c && !pFound; c = c->pPrevLink)
				if (c->dwStateNo == 0x62) pFound = c;
			for (D2StatListStrc* c = pList->pMyStats; c && !pFound; c = c->pPrevLink)
				if (c->dwStateNo == 0x62) pFound = c;
			if (pFound) {
				D2Common_ExpireStatList_6FDB6E30(pFound);
				if (!(pFound->dwFlags & 0x80000000u)) D2Common_STATLIST_FreeStatListImpl_6FDB7050(pFound);
			}
		}
	}
	pUnit->dwFlagEx &= 0xfffffbffu;
}
