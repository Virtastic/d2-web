// 1.13c D2Common implementation batch: rows 0x314A0, 0x18DF0, 0x24320, 0x38E30, 0x391A0, 0x5A7D0,
// 0x1FF40, 0x4E3F0, 0x30590. Bodies are translated from the pinned 1.13c decompiles
// (build-wasm/ghidra-d2common/decomp). The other four rows this file used to carry (0x39B30, 0x3CDF0,
// 0x4E5E0, 0x2F8A0) are already implemented by main's d2common_batch_b59.cpp, which owns those RVAs;
// their duplicates were dropped from this tree.
// Retail pointers are four bytes wide in the offsets below, so struct-like arguments are read at
// their retail byte offsets as raw 32-bit image addresses, as in the other d2common batch files.
// Callees that are still UNMAPPED rows are env-seam function pointers (null until the environment
// installs them); retail's fatal Fog paths (ordinal 10265 assert + ordinal 10024 halt) are modelled
// as the function's neutral return.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Inventory.h"
#include "D2Items.h"
#include "D2StatList.h"
#include "DataTbls/ItemsTbls.h"
#include "Units/Units.h"

static_assert(offsetof(D2ItemsTxt, wType) == 0x11E, "items.txt wType");
static_assert(sizeof(D2ItemTypesTxt) == 0xE4, "itemtypes.txt stride");
static_assert(offsetof(D2ItemTypesTxt, nClass) == 0x21, "itemtypes.txt nClass");
static_assert(sizeof(D2ItemStatCostTxt) == 0x144, "itemstatcost.txt stride");

extern D2InventoryGridInfoStrc gBodyLocInventoryGridInfo;

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t RdU32(const uint8_t* b, int o) { return *reinterpret_cast<const uint32_t*>(b + o); }
	inline int16_t RdI16(const uint8_t* b, int o) { return *reinterpret_cast<const int16_t*>(b + o); }
	inline uint8_t RdU8(const uint8_t* b, int o) { return *(b + o); }
	inline uint32_t& W(uint8_t* b, int o) { return *reinterpret_cast<uint32_t*>(b + o); }

}

// Env seams for callees that are still UNMAPPED rows, in retail argument order.
extern "C"
{
	void* (*g_d2lang_fn_10003)(int32_t);                                    // D2Lang ordinal 10003
	void* (*g_d2common_fn_10045)(const char*, int32_t, int32_t);            // Fog ordinal 10045
}

// 1.13c D2Common+0x18DF0 (#10840): bound check of a 0x34-byte record. Retail fatals 0x147 on a
// null record; there is nothing else to do.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_batch_d2common_18df0(int32_t nIndex)
{
	const uint8_t* pRecord = reinterpret_cast<const uint8_t*>(sgptDataTables->pCompositTxt) + static_cast<uintptr_t>(static_cast<uint32_t>(nIndex)) * 0x34;
	(void)pRecord;   // retail fatals 0x147 here; modelled as a no-op
}

// 1.13c D2Common+0x24320 (#10844): the items.txt wType[0] of nItemId indexes itemtypes.txt and the
// answer is nClass (+0x21) < 7. Out-of-range item ids are retail's fatal 0x68E -> false.
extern "C"
EMSCRIPTEN_KEEPALIVE bool D2Common_24320_ItemClassBelow7(uint32_t nItemId)
{
	const D2ItemsTxt* pItemRow = DATATBLS_GetItemsTxtRecord(static_cast<int32_t>(nItemId));
	if (!pItemRow)
		return false;

	const int32_t nType = RdI16(reinterpret_cast<const uint8_t*>(pItemRow), 0x11E);
	if (nType < 0 || nType >= sgptDataTables->nItemTypesTxtRecordCount || !sgptDataTables->pItemTypesTxt)
		return false;

	return RdU8(reinterpret_cast<const uint8_t*>(&sgptDataTables->pItemTypesTxt[nType]), 0x21) < 7;
}


// 1.13c D2Common+0x391a0 (#10848): total of one itemstatcost stat on a stat list. Null list or an
// out-of-range stat id answers 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_391a0(uint32_t pStatList, int32_t nStatId)
{
	if (pStatList == 0 || nStatId < 0 || nStatId >= sgptDataTables->nItemStatCostTxtRecordCount ||
	    !sgptDataTables->pItemStatCostTxt)
		return 0;

	return STATLIST_GetTotalStat_6FDB63E0(reinterpret_cast<D2StatListStrc*>(P(pStatList)), nStatId,
	                                      &sgptDataTables->pItemStatCostTxt[nStatId]);
}

// 1.13c D2Common+0x1FF40 (#10854): report the first usable item among the two hand slots of an
// inventory (body-loc grid cells +0x10 then +0x14) that is of item type 0x33. Cell 4 is taken
// only when it is a non-item-type-4 unit or a type-4 unit whose itemdata flags lack 0x100 and
// 0x4000; cell 5 must pass ITEMS_CanBeEquipped. 
int32_t D2Common_1FF40_GetHandAmmoItem(D2InventoryStrc* pInventory, D2UnitStrc** ppItem)
{
	if (!pInventory)
		return 0;

	if (ppItem)
		*ppItem = nullptr;

	D2UnitStrc* pItem = nullptr;
	if (pInventory->dwSignature == 0x01020304u)
	{
		D2InventoryGridStrc* pGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
		if (pGrid)
		{
			pItem = pGrid->ppItems[4];
			if (pItem)
			{
				const D2ItemDataStrc* d = pItem->dwUnitType == UNIT_ITEM ? pItem->pItemData : nullptr;
				if ((pItem->dwUnitType != UNIT_ITEM || ((!d || !(d->dwItemFlags & 0x100)) && (!d || !(d->dwItemFlags & 0x4000))))
					&& ITEMS_CheckItemTypeId(pItem, 0x33))
				{
					if (ppItem)
						*ppItem = pItem;
					return 1;
				}
			}
			pItem = pGrid->ppItems[5];
		}
	}

	if (!ITEMS_CanBeEquipped(pItem) || !ITEMS_CheckItemTypeId(pItem, 0x33))
		return 0;
	if (ppItem)
		*ppItem = pItem;
	return 1;
}

// 1.13c D2Common+0x4E3F0 (#10856): allocate and zero the 24-byte skill record through Fog ordinal
// 10045 (Skills.cpp:0x380), then store the first dword.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_4e3f0(uint32_t nValue)
{
	uint8_t* p = g_d2common_fn_10045
		? reinterpret_cast<uint8_t*>(g_d2common_fn_10045("Skills.cpp", 0x380, 0))
		: nullptr;
	if (!p)
		return;

	std::memset(p, 0, 24);
	W(p, 0) = nValue;
}

// 1.13c D2Common+0x30590 (#10859): refresh the item data's grid class from 0x30460's record S = [[unit+0xA8]+8]:
// the int16 behind the pointer S[0] becomes itemdata+0x84, and S's dword +0x34 becomes itemdata+0x8C. A unit
// with type != 0 is a no-op; no item data is retail's fatal 0x1424, a null record fatal 0x638.
extern "C"
EMSCRIPTEN_KEEPALIVE void D2Common_30590_RefreshGridClass(uint32_t pUnit)
{
	uint8_t* p = P(pUnit);
	if (!p || RdU32(p, 0) != 0)
		return;

	uint8_t* pItemData = P(RdU32(p, 0x14));
	if (!pItemData)
		return;   // retail fatals 0x1424

	const uint32_t nList = RdU32(p, 0xA8);
	const uint32_t nRecord = nList ? RdU32(P(nList), 8) : 0;
	const uint32_t pClass = nRecord ? RdU32(P(nRecord), 0) : 0;
	W(pItemData, 0x84) = pClass ? static_cast<uint32_t>(RdI16(P(pClass), 0)) : 0;
	if (!nRecord)
		return;   // retail fatals 0x638

	W(pItemData, 0x8C) = RdU32(P(nRecord), 0x34);
}
