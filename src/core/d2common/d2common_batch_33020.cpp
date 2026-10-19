// Retail 1.13c item wrappers cross-checked against the cached decompiles.
// Pointer fields use native D2MOO structs; scalar IDs retain retail widths.
#include <cstdint>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Items.h"
#include "D2StatList.h"
#include "Units/Units.h"

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
