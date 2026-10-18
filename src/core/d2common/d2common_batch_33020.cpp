// 1.13c D2Common implementation batch (rows 0x33020, 0x22920, 0x229A0).
// Each body delegates to the vendored D2MOO 1.10f function whose behaviour was matched line by
// line against the 1.13c decompile and disassembly; the retail offsets named in the comments are
// the x86 ones.
#include <cstdint>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Items.h"
#include "D2StatList.h"
#include "Units/Units.h"

// 1.13c D2Common+0x33020 (#10210): UNITS_StoreOwner. With an owner, the owner's type (unit+0x00)
// and GUID (unit+0x0C) are copied through UNITS_StoreOwnerInfo (retail 0x32CA0); without one the
// unit's owner fields (+0x94/+0x98) are cleared, state 98 (STATE_SOURCEUNIT) is toggled off, its
// stat list is expired and freed, and UNITFLAGEX_STOREOWNERINFO (+0xC8 bit 0x400) is cleared.
EMSCRIPTEN_KEEPALIVE void D2Common_10210_UNITS_StoreOwner(D2UnitStrc* pUnit, D2UnitStrc* pOwner)
{
	UNITS_StoreOwner(pUnit, pOwner);
}

// 1.13c D2Common+0x22920 (#10211): ItemsTxt row `nItemId`, byte at +0x137 (nHasInv), or 0 when the
// id is out of range. Retail reads the loader's contiguous table directly; the bounds check is the
// same unsigned compare DATATBLS_GetItemsTxtRecord (1.13c+0x719A0) performs.
EMSCRIPTEN_KEEPALIVE int D2Common_10211_ITEMS_CheckIfSocketableByItemId(int nItemId)
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
EMSCRIPTEN_KEEPALIVE BOOL D2Common_10217_ITEMS_CheckBodyLocation(D2UnitStrc* pItem, uint8_t nBodyLoc)
{
	return ITEMS_CheckBodyLocation(pItem, nBodyLoc);
}

// 1.13c D2Common+0x23F40 (#10218): ITEMS_GetSetItemsTxtRecordFromItem. Only for an item
// (unit+0x00 == 4) with item data (unit+0x14) whose quality (+0x00) is set (5); returns
// SetItemsTxt[pItemData->dwFileIndex (+0x28)] when the index is in range, else null.
EMSCRIPTEN_KEEPALIVE D2SetItemsTxt* D2Common_10218_ITEMS_GetSetItemsTxtRecordFromItem(D2UnitStrc* pItem)
{
	return ITEMS_GetSetItemsTxtRecordFromItem(pItem);
}
