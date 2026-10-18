// 1.13c D2Common implementation batch (rows 0x232E0, 0x1ED00, 0x2FD50, 0x32890, 0x50090,
// 0x66410, 0x46920, 0x3BDA0, 0x81F20, 0x386E0, 0x4E680, 0x50D40).
// Bodies follow the cached 1.13c decompiles (build-wasm/ghidra-d2common/decomp/0x<RVA>.c). Callees
// that retail reaches with operands the decompiler drops (hidden EAX/EBX/ECX) become env seams after
// the retail arguments; a null seam contributes 0 and no body faults on it. Fatal paths that end in
// _exit(-1) are modelled as returns.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Inventory.h"
#include "D2Items.h"
#include "D2Skills.h"
#include "D2StatList.h"
#include "DataTbls/ItemsTbls.h"
#include "DataTbls/SkillsTbls.h"
#include "Units/Item.h"
#include "Units/Units.h"
#include "Fog.h"

static_assert(offsetof(D2SkillsTxt, dwParam) == 0x148, "skills.txt dwParam");
static_assert(offsetof(D2ItemsTxt, dwWeapClass2Hand) == 0xC4, "items.txt dwWeapClass2Hand");
static_assert(offsetof(D2StatStrc, nValue) == 0x04, "stat value");

uint32_t D2Common_33C20_BitmaskHit(bool hasOwner, bool hasRec, int32_t recFlag,
	const uint32_t* bits, const uint32_t* mask, int32_t nWords);

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t A(const void* p) { return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p)); }
	inline uint32_t& W(uint8_t* b, int32_t o) { return *reinterpret_cast<uint32_t*>(b + o); }

	inline uint8_t* StatListOf(uint8_t* u) { return u ? P(W(u, 0x5C)) : nullptr; }

	// Total of one itemstatcost row on the unit's stat list (0 when either side is absent).
	inline int32_t StatTotal(uint8_t* u, int32_t nStat)
	{
		if (!sgptDataTables || !sgptDataTables->pItemStatCostTxt ||
			nStat < 0 || nStat >= sgptDataTables->nItemStatCostTxtRecordCount)
			return 0;
		uint8_t* sl = StatListOf(u);
		if (!sl)
			return 0;
		return STATLIST_GetTotalStat_6FDB63E0(
			reinterpret_cast<D2StatListStrc*>(sl),
			D2SLayerStatIdStrc::MakeFromStatId(static_cast<uint16_t>(nStat)).nPackedValue,
			&sgptDataTables->pItemStatCostTxt[nStat]);
	}

	// 0x382F0: copy the run of stats whose itemstatcost id is nStatId into pOut (packed+value pairs),
	// at most nMax, stopping at the first different id. The hidden EBX search key is (layer 0, id).
	int32_t CollectStats(uint8_t* sl, int32_t nStatId, void* pOut, int32_t nMax)
	{
		uint8_t* arr = (static_cast<int32_t>(W(sl, 0x10)) < 0) ? sl + 0x48 : sl + 0x24;
		uint8_t* pStat = P(W(arr, 0x00));
		const int32_t nCount = *reinterpret_cast<int16_t*>(arr + 0x04);
		if (!pStat || nCount <= 0)
			return 0;
		// 0x38220: lower bound of the packed key (layer 0, id) over the packed stat array.
		int32_t lo = 0, hi = nCount;
		const int32_t nKey = nStatId << 16;
		while (lo < hi)
		{
			const int32_t mid = (lo + hi) / 2;
			const int32_t v = static_cast<int32_t>(W(pStat, mid * 8));
			if (v < nKey) lo = mid + 1;
			else if (v > nKey) hi = mid;
			else { lo = mid; hi = mid; }
		}
		if (lo >= nCount)
			return 0;
		uint8_t* out = static_cast<uint8_t*>(pOut);
		int32_t nCopied = 0;
		for (int32_t i = lo; i < nCount && nCopied < nMax; ++i)
		{
			const uint32_t nPacked = W(pStat, i * 8);
			if ((nPacked >> 16) != static_cast<uint32_t>(nStatId))
				break;
			out[0] = static_cast<uint8_t>(nPacked);
			out[1] = static_cast<uint8_t>(nPacked >> 8);
			out[2] = static_cast<uint8_t>(nPacked >> 16);
			out[3] = static_cast<uint8_t>(nPacked >> 24);
			W(out, 4) = W(pStat, i * 8 + 4);
			out += 8;
			++nCopied;
		}
		return nCopied;
	}
}

// 1.13c D2Common+0x232E0 (#10664): two-handed weapon class of an item unit (items.txt +0xC4).
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_232e0(uint32_t pUnit)
{
	uint8_t* u = P(pUnit);
	if (!u || W(u, 0) != 4)
		return 0;
	const D2ItemsTxt* pTxt = DATATBLS_GetItemsTxtRecord(static_cast<int32_t>(W(u, 4)));
	if (!pTxt)
		return 0;
	return pTxt->dwWeapClass2Hand;
}

// 1.13c D2Common+0x1ED00 (#10668): walk an inventory grid for the first item whose class id equals
// the items.txt link of nItemCode and whose raw value at unit+0x2C (+0xC dword for modes 2 and 4..5,
// +0x02 word otherwise) is below 0xB. nGrid/nGridInfo are the hidden 0x1E1B0 operands (inventory.txt
// grid); pStopItem, when non-null, bounds the scan to the item before it. The struct behind unit+0x2C
// is unnamed in 1.13c, so it is read raw. The unit chain runs through item data (+0x14) +0x70.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_1ed00(uint32_t pInventory, int32_t nItemCode,
	uint32_t pStopItem, int32_t nGrid, uint32_t pGridInfo)
{
	uint8_t* inv = P(pInventory);
	if (!inv || W(inv, 0) != 0x1020304)
		return 0;

	const int32_t nLink = sgptDataTables && sgptDataTables->pItemsLinker
		? FOG_GetLinkIndex(sgptDataTables->pItemsLinker, static_cast<uint32_t>(nItemCode), 0)
		: -1;
	if (nLink < 0 || !DATATBLS_GetItemsTxtRecord(nLink))
		return 0;

	D2InventoryGridStrc* pGrid = INVENTORY_GetGrid(
		reinterpret_cast<D2InventoryStrc*>(inv), nGrid,
		reinterpret_cast<D2InventoryGridInfoStrc*>(P(pGridInfo)));
	if (!pGrid)
		return 0;
	uint32_t nItem = A(pGrid->pItem);

	// Skip everything before the stop item, validating each link.
	while (pStopItem && nItem != 0)
	{
		if (nItem == pStopItem)
			break;
		uint8_t* it = P(nItem);
		if (W(it, 0) != 4)
			return 0;
		const uint32_t nData = W(it, 0x14);
		if (!nData || nData == 0xFFFFFFA4u)
			return 0;
		nItem = W(P(nData), 0x70);
	}
	if (pStopItem && nItem == 0)
		return 0;

	while (nItem)
	{
		uint8_t* it = P(nItem);
		if (W(it, 4) == static_cast<uint32_t>(nLink))
		{
			const uint32_t nType = W(it, 0);
			uint32_t nValue;
			if (nType == 2 || (nType > 3 && nType < 6))
				nValue = W(P(W(it, 0x2C)), 0x0C);
			else if (W(it, 0x2C) == 0)
				return nItem;
			else
				nValue = *reinterpret_cast<uint16_t*>(P(W(it, 0x2C)) + 2);
			if (nValue < 0xB)
				return nItem;
		}
		if (W(it, 0) != 4)
			return 0;
		const uint32_t nData = W(it, 0x14);
		if (!nData || nData == 0xFFFFFFA4u)
			return 0;
		nItem = W(P(nData), 0x70);
	}
	return 0;
}

// 1.13c D2Common+0x2FD50 (#10669): for unit types 0, 1 and 3, the dword at path(+0x2C)+0x60 when
// path+0x58 is non-zero. The other paths are fatal in retail and yield 0 here.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_2fd50(uint32_t pUnit)
{
	uint8_t* u = P(pUnit);
	if (!u)
		return 0;
	const uint32_t nType = W(u, 0);
	if (!(nType == 0 || nType == 1 || nType == 3))
		return 0;
	uint8_t* p = P(W(u, 0x2C));
	if (!p || W(p, 0x58) == 0)
		return 0;
	return W(p, 0x60);
}

// 1.13c D2Common+0x32890 (#10672): scaled stat total. Base = total(31) + total(2)/4;
// bonus = total(171) + total(16); for unit types 0, 1 and 3 that carry state 101 the first matching
// list node (state number at node+0x14, chains at +0x3C and +0x40 via +0x2C) adds the two stats
// 350/351 scaled by a skill (+0x138) through the hidden callees. The result is
// base + (bonus*base)/100 with the /100 done as a 32-bit truncating division after the widening
// retail magic, then the stat-182 percent branches. Hidden operands are env seams.
struct D2Common32890Env
{
	uint32_t (*fn37DB0)(uint32_t pUnit, int32_t nState);
	uint32_t (*fn1250)(uint32_t pUnit);
	void (*fn1FF40)(uint32_t pInventory, int32_t* pOut);
	int32_t (*fn87080)(uint32_t pUnit, uint32_t nSkill, int32_t nTotalA, int32_t nTotalB);
};

// 1.13c D2Common+0x32890
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_32890(uint32_t pUnit, const D2Common32890Env* pEnv)
{
	uint8_t* u = P(pUnit);
	uint8_t* sl = StatListOf(u);
	const int32_t nCount = sgptDataTables ? sgptDataTables->nItemStatCostTxtRecordCount : 0;

	int32_t nBase = 0;
	if (u && sl && nCount >= 0x20)
		nBase = StatTotal(u, 31);
	int32_t nDex = 0;
	if (u && sl && nCount >= 3)
		nDex = StatTotal(u, 2);
	nBase += nDex / 4;

	int32_t nTotalB = 0;
	if (u && sl && nCount >= 0xAC)
		nTotalB = StatTotal(u, 171);
	if (u && sl && nCount >= 0x11)
		nTotalB += StatTotal(u, 16);

	const uint32_t nType = u ? W(u, 0) : 0xFFFFFFFFu;
	if (u && static_cast<int32_t>(nType) >= 0 && (nType < 2 || nType == 3) &&
		sgptDataTables && sgptDataTables->nStatesTxtRecordCount > 0x65 &&
		pEnv && pEnv->fn37DB0 && pEnv->fn37DB0(pUnit, 0x65) != 0 &&
		sl && static_cast<int32_t>(W(sl, 0x10)) < 0)
	{
		uint8_t* pNode = nullptr;
		for (uint32_t n = W(sl, 0x3C); n; n = W(P(n), 0x2C))
			if (W(P(n), 0x14) == 0x65) { pNode = P(n); break; }
		if (!pNode)
			for (uint32_t n = W(sl, 0x40); n; n = W(P(n), 0x2C))
				if (W(P(n), 0x14) == 0x65) { pNode = P(n); break; }
		if (pNode)
		{
			int32_t nA = nCount >= 0x15F ? StatTotal(u, 350) : 0;
			int32_t nB = nCount >= 0x160 ? StatTotal(u, 351) : 0;
			uint32_t nSkill = pEnv->fn1250 ? pEnv->fn1250(pUnit) : 0;
			int32_t nFlag = 0;
			if (nA > 0 && nB > 0 && nSkill &&
				pEnv->fn1FF40 && pEnv->fn87080)
			{
				pEnv->fn1FF40(W(u, 0x60), &nFlag);
				if (nFlag)
					nTotalB += pEnv->fn87080(pUnit, W(P(nSkill), 0x138), nA, nB);
			}
		}
	}

	// 32-bit wrapping product, then a truncating /100 (the local_c < 1 magic in retail).
	const int32_t nBonus = static_cast<int32_t>(static_cast<uint32_t>(nTotalB) *
		static_cast<uint32_t>(nBase)) / 100;
	uint32_t nResult = static_cast<uint32_t>(nBase + nBonus);

	uint32_t nPercent = 0;
	if (u && sl && nCount >= 0xB7)
		nPercent = static_cast<uint32_t>(StatTotal(u, 182));
	if (nPercent == 0)
		return nResult;
	if (static_cast<int32_t>(nResult) < 0x100001)
	{
		if (static_cast<int32_t>(nPercent) < 0x10001)
			return nResult + static_cast<int32_t>(nPercent * nResult) / 100;
		if (0x63F < static_cast<int32_t>(nPercent & 0xFFFFFFF0u))
			return nResult + static_cast<int32_t>(nPercent) / 100 * nResult;
	}
	else if (0x63F < static_cast<int32_t>(nResult & 0xFFFFFFF0u))
	{
		return nResult + static_cast<int32_t>(nResult) / 100 * nPercent;
	}
	return nResult + static_cast<uint32_t>(
		static_cast<int64_t>(static_cast<int32_t>(nPercent)) *
		static_cast<int64_t>(static_cast<int32_t>(nResult)) / 100);
}

// 1.13c D2Common+0x50090 (#10678): unit type 1 with mode 0xC whose stat-list flags hit the state
// mask table entry at datas+0x150 (fStateMasks[33]), or else whose hidden 0x51470(unit,7) is set.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_50090(uint32_t pUnit,
	uint32_t (*pfn51470)(uint32_t pUnit, int32_t nIndex))
{
	uint8_t* u = P(pUnit);
	if (!u || W(u, 0) != 1 || W(u, 0x10) != 0xC)
		return 0;
	uint8_t* sl = StatListOf(u);
	const uint32_t* pMask = sgptDataTables ? sgptDataTables->fStateMasks[33] : nullptr;
	const int32_t nWords = sgptDataTables ? (sgptDataTables->nStatesTxtRecordCount + 31) / 32 : 0;
	if (D2Common_33C20_BitmaskHit(sl != nullptr, sl != nullptr,
		sl ? static_cast<int32_t>(W(sl, 0x10)) : 0,
		sl ? reinterpret_cast<const uint32_t*>(P(W(sl, 0x58))) : nullptr,
		pMask, nWords) != 0)
		return 1;
	return (pfn51470 && pfn51470(pUnit, 7) != 0) ? 1 : 0;
}

// 1.13c D2Common+0x66410 (#10681): unlink the unit from its room's list (room+0x1C head, next at
// unit+0xE0). A missing room with a non-null unit+0xE0 is fatal in retail and yields no unlink here.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_66410(uint32_t pUnit)
{
	uint8_t* u = P(pUnit);
	if (!u)
		return;
	D2ActiveRoomStrc* pRoom = UNITS_GetRoom(reinterpret_cast<D2UnitStrc*>(u));
	uint8_t* r = reinterpret_cast<uint8_t*>(pRoom);
	if (!r)
		return;
	uint32_t nCur = W(r, 0x1C);
	uint32_t nPrev = 0;
	while (nCur && nCur != pUnit)
	{
		nPrev = nCur;
		nCur = W(P(nCur), 0xE0);
		if (!nCur)
			return;
	}
	if (!nCur)
		return;
	if (!nPrev)
		W(r, 0x1C) = W(P(nCur), 0xE0);
	else
		W(P(nPrev), 0xE0) = W(P(nCur), 0xE0);
	W(P(nCur), 0xE0) = 0;
}

// 1.13c D2Common+0x46920 (#10684): roll an item's file index from the table at DAT_6fdeefa4 (count
// DAT_6fdeefa0) with the item's seed (item data +4, LCG 0x6AC690C5), store it at item data +0x28,
// and on a value of 7 or 9 roll again into set-stat 0x47 ((low 2 bits + 1) * 0x100, else 1).
struct D2Common46920Env
{
	const int32_t* pTable;
	int32_t nCount;
};

// 1.13c D2Common+0x46920
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_46920(uint32_t pUnit, const D2Common46920Env* pEnv,
	void (*pfn38550)())
{
	uint8_t* u = P(pUnit);
	if (!u || W(u, 0) != 4)
		return;
	uint8_t* data = P(W(u, 0x14));
	if (!data)
		return;

	const int32_t nCount = pEnv ? pEnv->nCount : 0;
	const int32_t* pTable = pEnv ? pEnv->pTable : nullptr;
	uint64_t* pSeed = reinterpret_cast<uint64_t*>(data + 4);
	uint32_t nIndex = 0;
	if (nCount >= 1)
	{
		const uint64_t nNew = static_cast<uint64_t>(*reinterpret_cast<uint32_t*>(data + 4)) *
			0x6AC690C5ull + *reinterpret_cast<uint32_t*>(data + 8);
		*pSeed = nNew;
		if ((nCount & (nCount - 1)) == 0)
			nIndex = static_cast<uint32_t>(static_cast<uint32_t>(nCount - 1) & nNew);
		else
			nIndex = static_cast<uint32_t>(nNew) % static_cast<uint32_t>(nCount);
	}
	const int32_t nValue = pTable ? pTable[nIndex] : 0;
	W(data, 0x28) = static_cast<uint32_t>(nValue);

	int32_t nStatValue;
	if (nValue == 7 || nValue == 9)
	{
		const uint64_t nNew = static_cast<uint64_t>(*reinterpret_cast<uint32_t*>(data + 4)) *
			0x6AC690C5ull + *reinterpret_cast<uint32_t*>(data + 8);
		*pSeed = nNew;
		nStatValue = (static_cast<int32_t>(static_cast<uint32_t>(nNew) & 3) + 1) * 0x100;
	}
	else
	{
		nStatValue = 1;
	}

	D2StatListStrc* pStatList = reinterpret_cast<D2StatListStrc*>(P(W(u, 0x5C)));
	int32_t nSet = pStatList ? STATLIST_SetStat(pStatList, 0x47, nStatValue, 0) : 0;
	if (nSet != 0 && W(u, 0) == 0 && pfn38550)
		pfn38550();
}

// 1.13c D2Common+0x3BDA0 (#10685, D2GetUniqueID): 1 for unique mode 1, the 0x80000 bit of the
// unique data flags for mode 2, otherwise 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2GetUniqueID(uint32_t pUnit)
{
	if (!pUnit)
		return 0;
	uint8_t* p = P(W(P(pUnit), 0x10));
	if (!p)
		return 0;
	const int32_t nMode = static_cast<int32_t>(W(p, 0x48));
	if (nMode == 1)
		return 1;
	if (nMode == 2)
		return W(p, 0x28) & 0x80000;
	return 0;
}

// 1.13c D2Common+0x81F20 (#10701, GetGlobalEquipmentSlotLayout): five dwords at
// (nIndex*5+10)*4 of record (nSub*0x10 + nRow) in the 0xF0-stride layout table.
struct D2Common81F20Env
{
	const uint8_t* pLayouts;
	int32_t nRecords;
};

// 1.13c D2Common+0x81F20
extern "C"
EMSCRIPTEN_KEEPALIVE void GetGlobalEquipmentSlotLayout(int32_t nRow, int32_t nSub,
	uint32_t* pOut, int32_t nIndex, const D2Common81F20Env* pEnv)
{
	if (!pEnv || !pEnv->pLayouts || !pOut)
		return;
	const uint32_t nRecord = static_cast<uint32_t>(nSub) * 0x10 + static_cast<uint32_t>(nRow);
	if (static_cast<uint32_t>(pEnv->nRecords) < nRecord)
		return;
	const uint8_t* p = pEnv->pLayouts + nRecord * 0xF0 + (nIndex * 5 + 10) * 4;
	for (int32_t i = 0; i < 5; ++i)
		pOut[i] = W(const_cast<uint8_t*>(p), i * 4);
}

// 1.13c D2Common+0x386E0 (#10702): collect up to nMax stats of itemstatcost id nStatId from the
// unit's extended stat list into pOut (packed+value pairs); returns the count copied.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_386e0(uint32_t pUnit, int32_t nStatId,
	void* pOut, int32_t nMax)
{
	uint8_t* u = P(pUnit);
	if (!u)
		return 0;
	uint8_t* sl = P(W(u, 0x5C));
	if (!sl || static_cast<int32_t>(W(sl, 0x10)) >= 0)
		return 0;
	return CollectStats(sl, nStatId, pOut, nMax);
}

// 1.13c D2Common+0x4E680 (#10703): skills.txt record nSkillId's dwParam[0..1] scaled by
// D2Common_11033(nLevel, param0, param1); 0 for a bad id or a non-positive level.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_4e680(int32_t nLevel, int32_t nSkillId)
{
	if (!sgptDataTables || !sgptDataTables->pSkillsTxt || nSkillId < 0 ||
		nSkillId >= sgptDataTables->nSkillsTxtRecordCount || nLevel <= 0)
		return 0;
	const D2SkillsTxt* pTxt = &sgptDataTables->pSkillsTxt[nSkillId];
	return D2Common_11033(nLevel, static_cast<int32_t>(pTxt->dwParam[0]),
		static_cast<int32_t>(pTxt->dwParam[1]));
}

// 1.13c D2Common+0x50D40 (#10709, D2GetFlags): non-player units return 0x1B with stat 157 in *pOut
// when it is positive, else 0x29 with stat 158, else 0. Players resolve their selection (hidden
// 0x81B90(unit,1)) and its mode (hidden 0x72430): mode 1 takes stat 157 or 158 (0 when neither is
// positive); mode 7 takes stat 157, else stat 158 (0x29), else 0x1F; any other mode 0xFFFFFFFF.
struct D2Common50D40Env
{
	uint32_t (*fn81B90)(uint32_t pUnit, int32_t nIndex);
	uint32_t (*fn72430)(uint32_t pSelection);
};

// 1.13c D2Common+0x50D40
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2GetFlags(uint32_t pUnit, int32_t* pOut,
	const D2Common50D40Env* pEnv)
{
	uint8_t* u = P(pUnit);
	if (!u)
		return 0;
	uint8_t* sl = StatListOf(u);
	const int32_t nCount = sgptDataTables ? sgptDataTables->nItemStatCostTxtRecordCount : 0;

	if (W(u, 0) != 0)
	{
		if (sl && nCount > 0x9D)
		{
			const int32_t n157 = StatTotal(u, 157);
			if (n157 > 0)
			{
				if (pOut) *pOut = n157;
				return 0x1B;
			}
		}
		if (sl && nCount > 0x9E)
		{
			const int32_t n158 = StatTotal(u, 158);
			if (n158 > 0)
			{
				if (pOut) *pOut = n158;
				return 0x29;
			}
		}
		return 0;
	}

	if (!pEnv || !pEnv->fn81B90)
		return 0;
	const uint32_t nSel = pEnv->fn81B90(pUnit, 1);
	if (!nSel)
		return 0xFFFFFFFFu;
	const uint32_t nMode = pEnv->fn72430 ? (pEnv->fn72430(nSel) & 0xFFFF) : 0;

	if (nMode == 1)
	{
		if (sl && nCount >= 0x9E)
		{
			const int32_t n157 = StatTotal(u, 157);
			if (n157 > 0)
			{
				if (pOut) *pOut = n157;
				return 0x1B;
			}
		}
		const int32_t n158 = StatTotal(u, 158);
		if (n158 < 1)
			return 0;
		if (pOut) *pOut = n158;
		return 0x29;
	}
	if (nMode != 7)
		return 0xFFFFFFFFu;

	const int32_t n157 = sl ? StatTotal(u, 157) : 0;
	if (n157 < 1)
	{
		const int32_t n158 = sl ? StatTotal(u, 158) : 0;
		if (n158 < 1)
			return 0x1F;
		if (pOut) *pOut = n158;
		return 0x29;
	}
	if (pOut) *pOut = n157;
	return 0x1B;
}
