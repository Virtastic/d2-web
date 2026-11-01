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

	// 0x10B0/0x1100: one LCG step, then the limited roll (the same arithmetic as batch 0x10B0).
	uint32_t RollLimited(uint32_t* pSeed, uint32_t nMax)
	{
		if (static_cast<int32_t>(nMax) < 1)
			return 0;

		const uint64_t lSeed = static_cast<uint64_t>(pSeed[0]) * 0x6AC690C5ull + pSeed[1];
		pSeed[0] = static_cast<uint32_t>(lSeed);
		pSeed[1] = static_cast<uint32_t>(lSeed >> 32);

		if ((nMax & (nMax - 1)) != 0)
			return static_cast<uint32_t>(lSeed) % nMax;

		return (nMax - 1) & static_cast<uint32_t>(lSeed);
	}
}

// Env seams for callees that are still UNMAPPED rows, in retail argument order.
extern "C"
{
	void* (*g_d2lang_fn_10003)(int32_t);                                    // D2Lang ordinal 10003
	void* (*g_d2common_fn_10045)(const char*, int32_t, int32_t);            // Fog ordinal 10045
}

// Retail globals this batch reads; zero is the only value available until the rows that fill them
// are mapped.
extern "C"
{
	uint32_t g_d2common_dat_6fdf13d8 = 0;   // 0x18DF0 record base, stride 0x34
	int32_t g_d2common_dat_6fdf0b7c = 0;    // 0x5A7D0 lookup count
	int16_t* g_d2common_dat_6fdf0b6c = nullptr; // 0x5A7D0 lookup table
	uint8_t g_d2common_dat_6fdf1484[8] = {};    // 0x5A7D0 fallback string
}

// 1.13c D2Common+0x314A0 (#10839): UNITS_TestCollisionWithUnit on raw units -- room from the
// first unit, both coordinate pairs, both X sizes, then 0x30730 (UNITS_TestCollision) with the
// collision mask. Null units are retail's fatal 0x1290/0x1291; no room answers 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_314a0(uint32_t pUnit1, uint32_t pUnit2, uint16_t nMask)
{
	uint8_t* p1 = P(pUnit1);
	uint8_t* p2 = P(pUnit2);
	if (!p1 || !p2)
		return 0;

	D2ActiveRoomStrc* pRoom = UNITS_GetRoom(reinterpret_cast<D2UnitStrc*>(p1));
	if (!pRoom)
		return 0;

	D2CoordStrc coords1 = {};
	D2CoordStrc coords2 = {};
	UNITS_GetCoords(reinterpret_cast<D2UnitStrc*>(p1), &coords1);
	UNITS_GetCoords(reinterpret_cast<D2UnitStrc*>(p2), &coords2);

	return UNITS_TestCollision(coords1.nX, coords1.nY, UNITS_GetUnitSizeX(reinterpret_cast<D2UnitStrc*>(p1)),
	                           coords2.nX, coords2.nY, UNITS_GetUnitSizeX(reinterpret_cast<D2UnitStrc*>(p2)),
	                           pRoom, nMask);
}

// 1.13c D2Common+0x18DF0 (#10840): bound check of a 0x34-byte record. Retail fatals 0x147 on a
// null record; there is nothing else to do.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_18df0(int32_t nIndex)
{
	const uint8_t* pRecord = P(g_d2common_dat_6fdf13d8 + static_cast<uint32_t>(nIndex) * 0x34);
	(void)pRecord;   // retail fatals 0x147 here; modelled as a no-op
}

// 1.13c D2Common+0x24320 (#10844): the items.txt wType[0] of nItemId indexes itemtypes.txt and the
// answer is nClass (+0x21) < 7. Out-of-range item ids are retail's fatal 0x68E -> false.
extern "C"
EMSCRIPTEN_KEEPALIVE bool d2_fn_d2common_24320(uint32_t nItemId)
{
	const D2ItemsTxt* pItemRow = DATATBLS_GetItemsTxtRecord(static_cast<int32_t>(nItemId));
	if (!pItemRow)
		return false;

	const int32_t nType = RdI16(reinterpret_cast<const uint8_t*>(pItemRow), 0x11E);
	if (nType < 0 || nType >= sgptDataTables->nItemTypesTxtRecordCount || !sgptDataTables->pItemTypesTxt)
		return false;

	return RdU8(reinterpret_cast<const uint8_t*>(&sgptDataTables->pItemTypesTxt[nType]), 0x21) < 7;
}

// 1.13c D2Common+0x38e30 (#10845): item stat 159 (0xC93C = 159 * 0x144) from the stat list at
// unit+0x5C. No unit, no stat list, or no itemstatcost row answers 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_38e30(uint32_t pUnit)
{
	const uint8_t* p = P(pUnit);
	if (!p)
		return 0;

	D2StatListStrc* pStatList = reinterpret_cast<D2StatListStrc*>(P(RdU32(p, 0x5C)));
	if (!pStatList || sgptDataTables->nItemStatCostTxtRecordCount < 160 || !sgptDataTables->pItemStatCostTxt)
		return 0;

	return STATLIST_GetTotalStat_6FDB63E0(pStatList, 159, &sgptDataTables->pItemStatCostTxt[159]);
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

// 1.13c D2Common+0x5A7D0 (#10852): roll once over the unit's seed-limited table (count at
// 0x6FDF0B7C, int16 entries at 0x6FDF0B6C); a non-zero entry is resolved through D2Lang ordinal
// 10003, and anything else falls back to the static default string at 0x6FDF1484.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_d2common_5a7d0(uint32_t pUnit)
{
	uint8_t* p = P(pUnit);
	if (!p)
		return g_d2common_dat_6fdf1484;

	if (g_d2common_dat_6fdf0b7c > 0)
	{
		const uint32_t nRoll = RollLimited(reinterpret_cast<uint32_t*>(p + 0x20),
		                                   static_cast<uint32_t>(g_d2common_dat_6fdf0b7c));
		if (g_d2common_dat_6fdf0b6c && g_d2common_dat_6fdf0b6c[nRoll] != 0 && g_d2lang_fn_10003)
		{
			if (void* pString = g_d2lang_fn_10003(g_d2common_dat_6fdf0b6c[nRoll]))
				return pString;
		}
	}

	return g_d2common_dat_6fdf1484;
}

// 1.13c D2Common+0x1FF40 (#10854): report the first usable item among the two hand slots of an
// inventory (body-loc grid cells +0x10 then +0x14) that is of item type 0x33. Cell 4 is taken
// only when it is a non-item-type-4 unit or a type-4 unit whose itemdata flags lack 0x100 and
// 0x4000; cell 5 must pass ITEMS_CanBeEquipped. 
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_1ff40(uint32_t pInventory, uint32_t pOutItem)
{
	uint8_t* p = P(pInventory);
	if (!p)
		return 0;

	uint8_t* pOut = P(pOutItem);
	if (pOut)
		W(pOut, 0) = 0;

	D2UnitStrc* pItem = nullptr;
	if (RdU32(p, 0) == 0x01020304u)
	{
		D2InventoryGridStrc* pGrid = INVENTORY_GetGrid(reinterpret_cast<D2InventoryStrc*>(p), INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
		if (pGrid)
		{
			pItem = pGrid->ppItems[4];
			if (pItem)
			{
				const uint8_t* u = reinterpret_cast<const uint8_t*>(pItem);
				const uint8_t* d = P(RdU32(u, 0x14));
				if ((RdU32(u, 0) != 4 || (!d || !(RdU32(d, 0x18) & 0x100)) && (!d || !(RdU32(d, 0x18) & 0x4000)))
					&& ITEMS_CheckItemTypeId(pItem, 0x33))
				{
					if (pOut)
						W(pOut, 0) = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pItem));
					return 1;
				}
			}
			pItem = pGrid->ppItems[5];
		}
	}

	if (!ITEMS_CanBeEquipped(pItem) || !ITEMS_CheckItemTypeId(pItem, 0x33))
		return 0;
	if (pOut)
		W(pOut, 0) = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pItem));
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

// 1.13c D2Common+0x30590 (#10859): refresh the item data's grid class from 0x30460's record: the
// int16 behind the pointer at unit+0xA8 -> +8 becomes itemdata+0x84, and the record's dword +0x34
// becomes itemdata+0x8C. A unit with type != 0 or no item data is retail's fatal 0x1424/0x638.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_30590(uint32_t pUnit)
{
	uint8_t* p = P(pUnit);
	if (!p || RdU32(p, 0) != 0)
		return;

	uint8_t* pItemData = P(RdU32(p, 0x14));
	if (!pItemData)
		return;   // retail fatals 0x1424

	// 0x30460: unit+0xA8 is the record; its dword +8 is a pointer whose int16 +0 is the grid class
	// and whose dword +0x34 is stored beside it.
	const uint32_t nRecord = RdU32(p, 0xA8);
	const uint32_t nGridClass = nRecord ? RdU32(P(nRecord), 8) : 0;
	W(pItemData, 0x84) = nGridClass ? static_cast<uint32_t>(RdI16(P(nGridClass), 0)) : 0;
	if (!nGridClass)
		return;   // retail fatals 0x638

	W(pItemData, 0x8C) = RdU32(P(nGridClass), 0x34);
}
