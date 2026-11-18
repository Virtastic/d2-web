// 1.13c D2Common Stage A implementation batch (RE-DONE alpha lane): rows 0x82030, 0x4FA30, 0x32330,
// 0x23400, 0x34450, 0x21DA0.
// Bodies follow the cached 1.13c decompiles (build-wasm/ghidra-d2common/decomp/0x<RVA>.c). Callees that
// still own an UNMAPPED row are env-seam function pointers (null seam contributes the function's neutral
// value). Retail's fatal paths (Fog error + _exit) are modelled as returns, as in the other batch files.
// Vendor fields are read by name because wasm64 pointers widen the vendored structs past retail's offsets.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#include "D2BitManip.h"
#include "D2DataTbls.h"
#include "D2Items.h"
#include "D2StatList.h"
#include "DataTbls/ItemsTbls.h"
#include "DataTbls/SkillsTbls.h"
#include "Units/UnitRoom.h"
#include "Units/Units.h"

static_assert(sizeof(D2SkillsTxt) == 0x23C, "skills.txt record stride");
static_assert(offsetof(D2SkillsTxt, wReqStr) == 0x176, "skills.txt wReqStr");
static_assert(offsetof(D2SkillsTxt, wReqDex) == 0x178, "skills.txt wReqDex");
static_assert(offsetof(D2SkillsTxt, wReqInt) == 0x17A, "skills.txt wReqInt");
static_assert(offsetof(D2SkillsTxt, wReqVit) == 0x17C, "skills.txt wReqVit");
static_assert(sizeof(D2ItemStatCostTxt) == 0x144, "itemstatcost.txt record stride");

// Env seams for callees that are still-UNMAPPED rows. 0x38B70 and 0x38A80 are defined in
// d2common_batch_38c20.cpp / d2common_batch_37ca0.cpp; the rest are declared here for this batch.
extern "C"
{
	// 0x38B70: unit's total value of ItemStatCost row nStatId.
	extern int32_t (*g_d2common_fn_38B70)(uint32_t, int32_t);
	// 0x38A80: total of one ItemStatCost row pointer; the stat-list operand is register-only in retail.
	extern int32_t (*g_d2common_fn_38A80)(void*);
	// 0x4F510: skill-level gate (unit, skill id).
	int32_t (*g_d2common_fn_4F510)(uint32_t, int32_t) = nullptr;
	// 0x218A0: class-skill lookup (position, 0, 0, 0) -> skill id.
	uint32_t (*g_d2common_fn_218A0)(int32_t, int32_t, int32_t, int32_t) = nullptr;
	// 0x37F40: allocator of the 0x80-byte block below; returns a retail address.
	uint32_t (*g_d2common_fn_37F40)(uint32_t) = nullptr;
}

// 0x3A650 is implemented (d2common_batch_37ca0.cpp).
extern "C" void D2Common_3A650_DropListEntries(uint32_t pList);

// Retail globals read by 0x82030. They are filled by the module's position-table loader; zero is the
// only value available until the rows that fill them are mapped.
extern "C"
{
	uint32_t g_d2common_dat_6fdefa58 = 0;   // largest valid record index
	uint32_t g_d2common_dat_6fdefa5c = 0;   // base address of the 0xF0-stride position records
}

// Retail 0x82030 (#11012; play path: DATATBLS_GetInventoryRect): copy the 16-byte record at (nSecond * 16 + nFirst) of the global
// inventory-position table into pOut. The index is unsigned; each record is 0xF0 bytes. A missing base
// is fatal 0x7F and a missing/out-of-range record fatal 0x8C in retail (Fog message, then _exit); both
// are modelled as a plain return of 0. Retail leaves the record's fourth word (the last one copied) in EAX.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Common_82030_GetGlobalInventoryPosition(int32_t nFirst, int32_t nSecond, uint32_t* pOut) {  // retail 0x82030 (#11012)
	const uint32_t nIndex = static_cast<uint32_t>(nSecond) * 0x10u + static_cast<uint32_t>(nFirst);
	if (g_d2common_dat_6fdefa5c == 0 || pOut == nullptr)
		return 0;                                   // retail fatal 0x7F
	if (nIndex > g_d2common_dat_6fdefa58)
		return 0;                                   // retail fatal 0x8C
	const uint32_t pRecord = g_d2common_dat_6fdefa5c + nIndex * 0xF0u;
	if (pRecord == 0)
		return 0;                                   // retail fatal 0x8C
	std::memcpy(pOut, reinterpret_cast<const void*>(static_cast<uintptr_t>(pRecord)), 4 * sizeof(uint32_t));
	return pOut[3];
}

// 1.13c D2Common+0x4FA30 (#11021): skill-requirement test.
// nSkillId must index skills.txt and the row's dwFlags[0] byte +5 must carry mask 0x04 (retail reads the
// byte at gdwBitMasks+8, i.e. gdwBitMasks[2]). The requirement level is the unit stat list's total of
// ItemStatCost row 12 (0xF30 / 0x144); it is 0 without a unit, a stat list, at least 13 ItemStatCost rows
// or the loaded table. 0x4F510(unit, skill) must not exceed it, and stats 0, 2, 1, 3 must each reach
// wReqStr (+0x176), wReqDex (+0x178), wReqInt (+0x17A) and wReqVit (+0x17C).
extern "C" EMSCRIPTEN_KEEPALIVE bool D2Common_4FA30_D2GetLevelID(D2UnitStrc* pUnit, int32_t nSkillId) {  // 1.13c D2Common+0x4FA30 (#11021)
	if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount || sgptDataTables->pSkillsTxt == nullptr)
		return false;

	const D2SkillsTxt* pRow = &sgptDataTables->pSkillsTxt[nSkillId];
	if (pRow == nullptr)
		return false;

	if ((static_cast<uint8_t>(gdwBitMasks[2]) & reinterpret_cast<const uint8_t*>(pRow)[5]) == 0)
		return false;

	int32_t nRequirement = 0;
	if (pUnit != nullptr && pUnit->pStatListEx != nullptr &&
	    sgptDataTables->nItemStatCostTxtRecordCount >= 0xD && sgptDataTables->pItemStatCostTxt != nullptr &&
	    g_d2common_fn_38A80 != nullptr)
	{
		nRequirement = g_d2common_fn_38A80(&sgptDataTables->pItemStatCostTxt[12]);
	}

	const uint32_t nUnit = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pUnit));
	const int32_t nSkillLevel = g_d2common_fn_4F510 != nullptr ? g_d2common_fn_4F510(nUnit, nSkillId) : 0;
	if (nSkillLevel > nRequirement)
		return false;

	int32_t nStat = g_d2common_fn_38B70 != nullptr ? g_d2common_fn_38B70(nUnit, 0) : 0;
	if (static_cast<int16_t>(pRow->wReqStr) > nStat)
		return false;
	nStat = g_d2common_fn_38B70 != nullptr ? g_d2common_fn_38B70(nUnit, 2) : 0;
	if (static_cast<int16_t>(pRow->wReqDex) > nStat)
		return false;
	nStat = g_d2common_fn_38B70 != nullptr ? g_d2common_fn_38B70(nUnit, 1) : 0;
	if (static_cast<int16_t>(pRow->wReqInt) > nStat)
		return false;
	nStat = g_d2common_fn_38B70 != nullptr ? g_d2common_fn_38B70(nUnit, 3) : 0;
	return static_cast<int16_t>(pRow->wReqVit) <= nStat;
}

namespace
{
	// 1.13c 0x32330's per-stat block: a unit with a stat list answers 0x38A80 of ItemStatCost row nRow,
	// provided the table has nMinRecords rows and a loaded base; anything else answers 0.
	int32_t StatListRowTotal(const D2UnitStrc* pUnit, int32_t nRow, int32_t nMinRecords)
	{
		if (pUnit == nullptr || pUnit->pStatListEx == nullptr)
			return 0;
		if (sgptDataTables->nItemStatCostTxtRecordCount < nMinRecords || sgptDataTables->pItemStatCostTxt == nullptr)
			return 0;
		if (g_d2common_fn_38A80 == nullptr)
			return 0;
		return g_d2common_fn_38A80(&sgptDataTables->pItemStatCostTxt[nRow]);
	}
}

// 1.13c D2Common+0x32330 (#11027): 8.8-fixed stat n at ItemStatCost row 6 as a percentage of the same
// stat at row 7, both shifted right by 8; a zero denominator (after the shift) answers 0.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t D2Common_32330_GetStatPercent(D2UnitStrc* pUnit) {  // 1.13c D2Common+0x32330 (#11027)
	const int32_t nNumerator = StatListRowTotal(pUnit, 6, 7) >> 8;
	const int32_t nDenominator = StatListRowTotal(pUnit, 7, 8) >> 8;
	if (nDenominator != 0)
		return (nNumerator * 100) / nDenominator;
	return 0;
}

// Export of 1.13c D2Common 0x23400 (#11036); the body is ITEMS_CanBeRare in src/core/game/Items/Items.cpp.
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t D2Common_23400_CanBeRare(D2UnitStrc* pItem) {
	return static_cast<uint8_t>(ITEMS_CanBeRare(pItem));
}

// 1.13c D2Common+0x34450 (#11039): set (nSet != 0) or clear a state bit in the unit's extended stat
// list. Only an extended list (dwFlags negative, bit 31) with a StatFlags array is valid; the bit lives
// in the second bitset, one word past ceil(nStates/32), masked by gdwBitMasks[nState & 31] on set and
// gdwInvBitMasks[nState & 31] on clear. Both paths refresh the unit's room membership (0x665C0).
// The body now lives in STATES_ToggleGfxStateFlag (src/core/game/D2States.cpp), same marker.

// D2Common ordinal 11041 (not the 0x21DA0 body-location placement): class-skill id at nPosition in the unit's class skill list, i.e.
// 0x218A0(nPosition, 0, 0, 0). A null unit or a position outside 0..0xC answers 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Common_21DA0_D2GetPlayerSkillID(D2UnitStrc* pUnit, uint32_t nUnused, int32_t nPosition) {  // 1.13c D2Common+0x21DA0 (#11041)
	(void)nUnused;
	if (pUnit == nullptr || nPosition < 0 || nPosition >= 0xD)
		return 0;
	if (g_d2common_fn_218A0 == nullptr)
		return 0;
	return g_d2common_fn_218A0(nPosition, 0, 0, 0);
}
