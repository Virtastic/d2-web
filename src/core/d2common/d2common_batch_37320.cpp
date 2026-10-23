// 1.13c D2Common Stage A implementation batch: 0x37320, 0x3BB80, 0x24E10, 0x246A0, 0x34390,
// 0x71430, 0x20BC0. Bodies are transliterated from the pinned 1.13c decompiles; records keep
// retail's 32-bit layout, so fields are read at their retail byte offsets. Retail's fatal asserts
// (_exit after a Fog message) are modelled as the function's neutral return, and callees whose
// recovered body lives behind an env seam are reached through that seam (null seam = unavailable).
#include <cstddef>
#include <cstdint>

#include <emscripten.h>

#include "D2DataTbls.h"
#include "DataTbls/ItemsTbls.h"
#include "D2Items.h"
#include "Units/Units.h"
#include "inv_place.h"

static_assert(sizeof(D2ItemTypesTxt) == 0xE4, "itemtypes.txt record stride");
// The itemtypes rows and count are the data tables' entries that sit at retail +0xBF8/+0xBFC; the
// vendored struct places them by name, so they are reached through sgptDataTables rather than raw.

// 0x260F0 (src/core/d2common/d2common_batch_343e0.cpp) and 0x22B70 (d2common_batch_22ec0.cpp).
extern "C" int32_t D2SetStatWithNoTest(uint32_t pUnit, int32_t (*pfn22B70)(uint32_t),
	int32_t (*pfn38A80)(uint32_t, uint32_t));
uint32_t D2Common_22B70_GetMaxStack(uint32_t nItemId);
uint32_t D2Common_230A0_ItemTypeFromItemId(uint32_t nItemId);

// 0x20BC0's inventory setup runs through the inventory seams of 0x81FB0 / 0x1E1B0, and its value
// compare calls 0x38A80 with an operand the decompiler drops. Both are installed here; a null seam
// is treated as the retail neutral value.
extern "C" const InvEnv* g_d2common_inv_env = nullptr;
extern "C" int32_t (*g_d2common_fn_38a80)(uint32_t) = nullptr;

extern "C" uint32_t* g_d2common_dat_6fdefd9c = nullptr;	// row base of the 0x22-byte table
extern "C" int32_t g_d2common_dat_6fdefd98 = 0;			// its record count

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& U(uint8_t* b, int o) { return *reinterpret_cast<uint32_t*>(b + o); }
	inline int32_t& I(uint8_t* b, int o) { return *reinterpret_cast<int32_t*>(b + o); }
	inline int16_t& S(uint8_t* b, int o) { return *reinterpret_cast<int16_t*>(b + o); }

	// D2InventoryStrc signature dword and pOwner dword.
	constexpr uint32_t INVENTORY_HEADER = 0x1020304;

	// The short at item data +0x3E, or 0 when the unit is not an item or has no item data.
	int16_t ItemShort(uint32_t pUnit)
	{
		uint8_t* u = P(pUnit);
		if (!u || U(u, 0x00) != 4) return 0;
		const uint32_t pData = U(u, 0x14);
		if (!pData) return 0;
		return S(P(pData), 0x3E);
	}

	int32_t MaxStackThunk(uint32_t nItemId) { return static_cast<int32_t>(D2Common_22B70_GetMaxStack(nItemId)); }
}

// 1.13c D2Common+0x37320: dword +0x28 of the record at *(param_1 + 4). A null param_1 is retail's
// fatal 0x547; the inner null record is not checked by retail and is modelled as 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_37320(uint32_t pRecord) {   // 1.13c D2Common+0x37320
	uint8_t* r = P(pRecord);
	if (!r) return 0;
	uint8_t* inner = P(U(r, 0x04));
	if (!inner) return 0;
	return U(inner, 0x28);
}

// 1.13c D2Common+0x3BB80: store param_2 at +0x08 of every node in the list at
// *(*(param_1 + 0x10) + 0x4C), following the next pointer at node +0x04. Null param_1 is retail's
// fatal 0x62; the null intermediate pointer is not dereferenced here.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_3bb80(uint32_t pRecord, uint32_t nValue) {   // 1.13c D2Common+0x3BB80
	uint8_t* r = P(pRecord);
	if (!r) return;
	uint8_t* outer = P(U(r, 0x10));
	if (!outer) return;
	for (uint32_t n = U(outer, 0x4C); n != 0; n = U(P(n), 0x04))
		U(P(n), 0x08) = nValue;
}

// 1.13c D2Common+0x24e10: itemtypes.txt nAutoStack (+0x13) of an item unit's item type. Anything
// but a type-4 unit, an out-of-range item type or a missing row answers 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint8_t d2_fn_d2common_24e10(uint32_t pUnit) {   // 1.13c D2Common+0x24E10
	uint8_t* u = P(pUnit);
	if (!u || U(u, 0x00) != 4) return 0;
	const int32_t nItemType = static_cast<int32_t>(ITEMS_GetItemType(reinterpret_cast<D2UnitStrc*>(u)));
	if (nItemType < 0 || nItemType >= sgptDataTables->nItemTypesTxtRecordCount) return 0;
	const D2ItemTypesTxt* pRow = sgptDataTables->pItemTypesTxt;
	if (!pRow) return 0;
	return pRow[nItemType].nAutoStack;
}

// 1.13c D2Common+0x246A0: itemtypes.txt nStaffMods (+0x1F) of an item unit's item type. Anything
// but a type-4 unit, an out-of-range item type or a missing row answers the retail default 7.
extern "C"
EMSCRIPTEN_KEEPALIVE uint8_t d2_fn_d2common_246a0(uint32_t pUnit) {   // 1.13c D2Common+0x246A0
	uint8_t* u = P(pUnit);
	if (!u || U(u, 0x00) != 4) return 7;
	const int32_t nItemType = static_cast<int32_t>(ITEMS_GetItemType(reinterpret_cast<D2UnitStrc*>(u)));
	if (nItemType < 0 || nItemType >= sgptDataTables->nItemTypesTxtRecordCount) return 7;
	const D2ItemTypesTxt* pRow = sgptDataTables->pItemTypesTxt;
	if (!pRow) return 7;
	return pRow[nItemType].nStaffMods;
}

// 1.13c D2Common+0x34390: when the record at param_1+0x5C exists and its +0x10 dword is negative,
// zero ceil(n/32) dwords starting ceil(n/32)*4 bytes into the buffer at that record's +0x58, where n
// is the dword at data tables +0xC4. The decompiler lost the count of the trailing byte loop, so
// only the dword loop below is modelled; the byte tail is unproven.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_34390(uint32_t pRecord) {   // 1.13c D2Common+0x34390
	uint8_t* r = P(pRecord);
	if (!r) return;
	const uint32_t pInner = U(r, 0x5C);
	if (!pInner) return;
	uint8_t* inner = P(pInner);
	if (I(inner, 0x10) >= 0) return;
	if (!sgptDataTables) return;

	const int32_t nBits = I(reinterpret_cast<uint8_t*>(sgptDataTables), 0xC4);
	const uint32_t nDwords = static_cast<uint32_t>((nBits + 0x1F + ((nBits + 0x1F) >> 31 & 0x1F)) >> 5);
	uint8_t* pDst = P(U(inner, 0x58) + nDwords * 4);
	if (!pDst) return;
	for (uint32_t i = 0; i < nDwords; ++i)
		U(pDst, static_cast<int>(i * 4)) = 0;
}

// 1.13c D2Common+0x71430: address of row param_1 in the 0x22-byte table (count at
// DAT_6fdefd98, base at DAT_6fdefd9C). Index -1 or one past the count answers 0; a null base or a
// null computed row is retail's fatal 0x5D0/0x5D1 and also answers 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_71430(int32_t nIndex) {   // 1.13c D2Common+0x71430
	if (nIndex >= g_d2common_dat_6fdefd98 || nIndex == -1) return 0;
	if (!g_d2common_dat_6fdefd9c) return 0;
	const int32_t nRow = static_cast<int32_t>(static_cast<uint32_t>(nIndex) * 0x22u +
		static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_d2common_dat_6fdefd9c)));
	return nRow;
}

// 1.13c D2Common+0x20BC0: scan a validated inventory's first item list for an item of item type
// 0x12 whose item-data +0x3E short matches param_2's and whose 0x260F0 value exceeds the 0x38A80
// total of the global stat list. param_3, when non-null, is the node to start after. The inventory
// grid of 0x1E1B0 arrives with inventory and page in ECX/EDX, which the decompile drops; the main
// page of param_1 is used here and that argument pair is unproven. The 0x38A80 operands the
// decompiler drops are passed as 0 by the seam signature.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_20bc0(uint32_t pInventory, uint32_t pCriteria, uint32_t pStart) {   // 1.13c D2Common+0x20BC0
	uint8_t* inv = P(pInventory);
	if (!inv || U(inv, 0x00) != INVENTORY_HEADER) return 0;

	const uint32_t pOwner = U(inv, 0x08);
	uint32_t pLayout = 0;
	if (pOwner && U(P(pOwner), 0x00) == 0) {
		const int32_t nRecord = UNITS_GetInventoryRecordId(reinterpret_cast<D2UnitStrc*>(P(pOwner)), 0, 1);
		uint8_t s_Layout[24];
		if (g_d2common_inv_env && D2Common_81FB0_GetLayout(*g_d2common_inv_env, nRecord, 0, s_Layout))
			pLayout = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(s_Layout));
	}

	uint32_t pGrid = 0;
	if (g_d2common_inv_env)
		pGrid = D2Common_1E1B0_GetGrid(*g_d2common_inv_env, pInventory, 0, pLayout);
	if (!pGrid) return 0;

	uint32_t pItem = U(P(pGrid), 0x00);
	if (pStart) {
		while (pItem) {
			if (pItem == pStart) break;
			if (U(P(pItem), 0x00) != 4) return 0;
			const uint32_t pData = U(P(pItem), 0x14);
			if (!pData || pData == 0xFFFFFFA4u) return 0;
			pItem = U(P(pData), 0x70);
		}
	}

	while (pItem && U(P(pItem), 0x00) == 4) {
		if (D2Common_230A0_ItemTypeFromItemId(U(P(pItem), 0x04)) == 0x12) {
			if (ItemShort(pItem) == ItemShort(pCriteria)) {
				int32_t nThreshold = 0;
				if (U(P(pItem), 0x5C) != 0 && sgptDataTables) {
					const int32_t nVer = I(reinterpret_cast<uint8_t*>(sgptDataTables), 0xBD4);
					const uint32_t pStatList = U(reinterpret_cast<uint8_t*>(sgptDataTables), 0xBCC) + 0x5898;
					if (nVer >= 0x47 && pStatList != 0 && g_d2common_fn_38a80)
						nThreshold = g_d2common_fn_38a80(pStatList);
				}
				const int32_t nValue = D2SetStatWithNoTest(pItem, &MaxStackThunk, nullptr);
				if (nThreshold < nValue) return pItem;
			}
		}
		if (U(P(pItem), 0x00) != 4) return 0;
		const uint32_t pData = U(P(pItem), 0x14);
		if (!pData || pData == 0xFFFFFFA4u) return 0;
		pItem = U(P(pData), 0x70);
	}
	return 0;
}
