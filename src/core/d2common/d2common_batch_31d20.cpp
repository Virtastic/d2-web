// 1.13c D2Common implementation batch (rows 0x31D20, 0x3BF60, 0x26DB0, 0x3A330, 0x30AC0,
// 0x5A810, 0x46770, 0x2F6A0, 0x5F260, 0x12140, 0x4E2B0, 0x84C30).
// Bodies follow the cached 1.13c decompiles (build-wasm/ghidra-d2common/decomp/0x<RVA>.c). Callees
// that retail reaches with operands the decompiler drops (hidden ECX/EDX/EBX) are env-seam function
// pointers after the retail arguments; a null seam contributes 0 and the body never faults on it.
// The fatal paths that end in _exit(-1) are modelled as returns, as in the rest of src/core/d2common.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2StatList.h"

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& W(uint8_t* b, int32_t o) { return *reinterpret_cast<uint32_t*>(b + o); }

	// 1.13c ItemStatCostTxt row pointer (stride 0x144), or null when the table is absent.
	inline uint32_t ItemStatRow(int32_t nRow)
	{
		if (!sgptDataTables->pItemStatCostTxt || sgptDataTables->nItemStatCostTxtRecordCount <= nRow)
			return 0;
		return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&sgptDataTables->pItemStatCostTxt[nRow]));
	}
}

// 1.13c D2Common+0x31D20 (#10212): block chance.
// Unit type 0: the inventory gate 0x1FF40 (unit+0x60, null arg) must succeed; the block total is the
// stat list (unit+0x5C) total of ItemStatCost row 20 (item_toblock) plus charstats[classId].nBlockFactor
// (row stride 0xC4, +0x49; an out-of-range classId reads row 0, as retail does). With nScaleByLevel the
// result becomes (dexterity - 15) * total / (level * 2), level floored at 1. Unit type 1: the monstats
// row (stride 0x1A8) byte +0xF bit 2 or 0x31680 must accept the unit, the stat list must exist and the
// block total is row 20 alone. Other types give 0. The result is capped at 75 (0x4B).
// 0x1FF40, 0x31680, 0x38B70 (unit stat getter) and 0x38A80 (whose stat-list operand is register-only)
// are env seams.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_31d20(uint32_t pUnit, int32_t nScaleByLevel,
	int32_t (*pfnInv1FF40)(uint32_t, void*),
	int32_t (*pfnUnit31680)(uint32_t),
	int32_t (*pfnStat38B70)(uint32_t, int32_t),
	int32_t (*pfnTotal38A80)(uint32_t, uint32_t))
{
	uint8_t* u = P(pUnit);
	const int32_t nType = u ? static_cast<int32_t>(W(u, 0)) : 6;
	int32_t nTotal;

	if (nType == 0)
	{
		if (!u || !pfnInv1FF40 || pfnInv1FF40(W(u, 0x60), nullptr) == 0)
			return 0;

		const uint32_t nStatList = W(u, 0x5C);
		const uint32_t pRow20 = ItemStatRow(20);
		if (nStatList == 0 || pRow20 == 0 || !pfnTotal38A80)
			nTotal = 0;
		else
			nTotal = pfnTotal38A80(nStatList, pRow20);

		int32_t nClass = static_cast<int32_t>(W(u, 4));
		if (nClass < 0 || nClass >= sgptDataTables->nCharStatsTxtRecordCount)
			nClass = 0;
		if (sgptDataTables->pCharStatsTxt)
			nTotal += reinterpret_cast<const uint8_t*>(&sgptDataTables->pCharStatsTxt[nClass])[0x49];

		if (nScaleByLevel)
		{
			const int32_t nDex = pfnStat38B70 ? pfnStat38B70(pUnit, 2) : 0;
			int32_t nLevel = pfnStat38B70 ? pfnStat38B70(pUnit, 0xC) : 0;
			if (nLevel < 2)
				nLevel = 1;
			nTotal = ((nDex - 0xF) * nTotal) / (nLevel * 2);
		}
	}
	else
	{
		if (nType != 1)
			return 0;

		const int32_t nClass = u ? static_cast<int32_t>(W(u, 4)) : -1;
		bool bMonsterOk = false;
		if (u && nClass >= 0 && nClass < sgptDataTables->nMonStatsTxtRecordCount && sgptDataTables->pMonStatsTxt)
			bMonsterOk = (reinterpret_cast<const uint8_t*>(&sgptDataTables->pMonStatsTxt[nClass])[0x0F] & 0x04) != 0;

		if (!bMonsterOk && (!pfnUnit31680 || pfnUnit31680(pUnit) == 0))
			return 0;

		const uint32_t nStatList = W(u, 0x5C);
		const uint32_t pRow20 = ItemStatRow(20);
		if (nStatList == 0 || pRow20 == 0 || !pfnTotal38A80)
			return 0;
		nTotal = pfnTotal38A80(nStatList, pRow20);
	}

	return nTotal < 0x4B ? nTotal : 0x4B;
}

// 1.13c D2Common+0x3BFB0 (#11025): room+0x10 -> data; data+0x28 flag 0x800000 clear -> level (data+0x58) +0x1D0.
// Null room returns 0; a null data block is fatal in retail (0x26B) and returns 0 here.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_3BFB0(uint32_t pRoom)
{
	uint8_t* r = P(pRoom);
	if (!r)
		return 0;
	uint8_t* d = P(W(r, 0x10));
	if (!d || (W(d, 0x28) & 0x800000))
		return 0;
	uint8_t* lv = P(W(d, 0x58));
	return lv ? W(lv, 0x1D0) : 0;
}

// 1.13c D2Common+0x3BF60 (#10233): unit+0x10 (the unit's data block) non-null and its dword +0x28
// carries 0x30000. A null unit or a null data block is fatal in retail (0x363 / 0x276); both return 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_3BF60(uint32_t pUnit)
{
	uint8_t* u = P(pUnit);
	if (!u)
		return 0;
	const uint32_t pData = W(u, 0x10);
	if (!pData)
		return 0;
	return (W(P(pData), 0x28) & 0x30000) != 0 ? 1u : 0u;
}

// 1.13c D2Common+0x26DB0 (#10244): item requirement check for the unit pUnit.
// Item unit only (type 4) with an items.txt row: wReqStr (+0x10A) and wReqDex (+0x10C), raised by the
// 0x11E0 ratio sum when the item's stat list (item+0x5C) carries ItemStatCost row 91 (0x732C/0x144),
// and lowered by 10 each when item data (+0x14) has 0x400000 at +0x18. Strength (stat 0) and dexterity
// (stat 2) each fail when 0, below the requirement, or below it after the 0x266E0 per-stat reduction
// (only with nCheckStats, a stat list owner whose +0x10 is negative and +0x44 non-zero). The 0x26190
// level requirement fails when it is not -1 and above the unit's level (stat 0xC). The outputs are the
// strength/dexterity/level flags in that order. The last gate needs all three flags and item data bit
// 0x10, and either 0x230F0 is not 0x12 or stat 0x46 is positive; then 0x24280's value 7, or a type-1
// unit at prop 4 with 0x51280 in 0x230..0x231, or a type-0 unit whose class id equals the prop.
// 0x11E0, 0x230F0, 0x24280, 0x26190, 0x266E0, 0x38A80, 0x38B70 and 0x51280 are env seams.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_26db0(uint32_t pItem, uint32_t pUnit, int32_t nCheckStats,
	int32_t* pOutStrOk, uint32_t* pOutDexOk, int32_t* pOutLevelOk,
	int32_t (*pfnRatio11E0)(int32_t, int32_t),
	int32_t (*pfnType230F0)(uint32_t),
	uint32_t (*pfnProp24280)(uint32_t),
	int32_t (*pfn26190)(uint32_t, uint32_t),
	int32_t (*pfn266E0)(uint32_t, int32_t, int32_t*),
	int32_t (*pfnTotal38A80)(uint32_t, uint32_t),
	int32_t (*pfnStat38B70)(uint32_t, int32_t),
	int32_t (*pfn51280)())
{
	uint8_t* item = P(pItem);
	if (pOutStrOk)
		*pOutStrOk = 0;
	if (pOutDexOk)
		*pOutDexOk = 0;
	if (pOutLevelOk)
		*pOutLevelOk = 0;

	if (!item || W(item, 0) != 4)
		return 0;
	const D2ItemsTxt* pRow = DATATBLS_GetItemsTxtRecord(static_cast<int32_t>(W(item, 4)));
	if (!pRow)
		return 0;

	const int32_t nReqStrBase = pRow->wReqStr;
	const int32_t nReqDexBase = pRow->wReqDex;
	const uint32_t pItemData = W(item, 0x14);
	const uint32_t nItemFlags = pItemData ? (W(P(pItemData), 0x18) & 0x10) : 0;
	const uint32_t nStatList = W(item, 0x5C);

	int32_t nStrOk = 1;
	int32_t nDexOk = 1;
	int32_t nLevelOk = 1;
	int32_t nStrBonus = 0;
	int32_t nDexBonus = 0;

	const uint32_t pRow91 = ItemStatRow(0x732C / 0x144);
	if (nStatList && pRow91 && pfnTotal38A80 && pfnTotal38A80(nStatList, pRow91) != 0)
	{
		nStrBonus = pfnRatio11E0 ? pfnRatio11E0(100, nReqStrBase) : 0;
		nDexBonus = pfnRatio11E0 ? pfnRatio11E0(100, nReqDexBase) : 0;
	}

	if (pItemData && (W(P(pItemData), 0x18) & 0x400000) != 0)
	{
		nStrBonus -= 10;
		nDexBonus -= 10;
	}

	int32_t nStr = (pUnit && pfnStat38B70) ? pfnStat38B70(pUnit, 0) : 0;
	if (nStr < 1)
	{
		nStrOk = 0;
	}
	else
	{
		const int32_t nReqStr = nStrBonus + nReqStrBase;
		if (nStr < nReqStr)
		{
			nStrOk = 0;
		}
		else if (nCheckStats && nStatList)
		{
			const uint32_t pOwner = W(P(nStatList), 0x34);
			if (pOwner && static_cast<int32_t>(W(P(pOwner), 0x10)) < 0 && W(P(pOwner), 0x44) != 0)
			{
				int32_t nReduction = 0;
				if (pfn266E0)
					pfn266E0(pItem, 0, &nReduction);
				if (nStr - nReduction < 1 || nStr - nReduction < nReqStr)
					nStrOk = 0;
			}
		}
	}

	int32_t nDex = (pUnit && pfnStat38B70) ? pfnStat38B70(pUnit, 2) : 0;
	if (nDex < 1)
	{
		nDexOk = 0;
	}
	else
	{
		const int32_t nReqDex = nDexBonus + nReqDexBase;
		if (nDex < nReqDex)
		{
			nDexOk = 0;
		}
		else if (nCheckStats)
		{
			int32_t nReduction = 0;
			if (pfn266E0)
				pfn266E0(pItem, 2, &nReduction);
			nDexOk = (nDex - nReduction > 0) ? 1 : 0;
			if (nDex - nReduction < nReqDex)
				nDexOk = 0;
		}
	}

	const int32_t nLevelReq = pfn26190 ? pfn26190(pItem, pUnit) : -1;
	if (nLevelReq != -1)
	{
		const int32_t nLevel = (pUnit && pfnStat38B70) ? pfnStat38B70(pUnit, 0xC) : 0;
		if (nLevel < nLevelReq)
			nLevelOk = 0;
	}

	if (pOutStrOk)
		*pOutStrOk = nStrOk;
	if (pOutDexOk)
		*pOutDexOk = static_cast<uint32_t>(nDexOk);
	if (pOutLevelOk)
		*pOutLevelOk = nLevelOk;

	if (!nStrOk || !nDexOk || !nLevelOk || !nItemFlags)
		return 0;

	const int32_t nType = pfnType230F0 ? pfnType230F0(pItem) : 0;
	if (nType == 0x12 && !(pfnStat38B70 && pfnStat38B70(pItem, 0x46) > 0))
		return 0;

	const uint32_t nProp = pfnProp24280 ? pfnProp24280(pItem) : 0;
	if (nProp == 7)
		return 1;

	uint8_t* pu = P(pUnit);
	if (!pu)
		return 0;

	const uint32_t nUnitType = W(pu, 0);
	if (nUnitType == 1 && nProp == 4)
	{
		const int32_t nArea = pfn51280 ? pfn51280() : 0;
		if (nArea > 0x22F && nArea < 0x232)
			return 1;
	}
	if (nUnitType == 0 && nProp == W(pu, 4))
		return 1;
	return 0;
}

// 1.13c D2Common+0x3A330 (#10245): add nDelta to a base stat of a stat list (packed layer/stat id in EDI).
// Find the entry in the list's base array (+0x24) with STATLIST_FindStatIndex (0x382B0); a missing entry is
// inserted (0x38640 = STATLIST_InsertStatOrFail); the value is adjusted; zero removes it (0x381A0);
// then sub_6FDB6C10 (0x39F70) with a null unit; list flags < 0 and owner type 0 end with 0x38550.
// All five callees are the vendored D2StatList.cpp functions, called directly.
uint32_t D2Common_3A330_AddStat(D2StatListStrc* pStatList, uint32_t nPackedStat, int32_t nDelta)
{
	if (!pStatList || nDelta == 0)
		return 0;

	D2StatsArrayStrc* pArray = &pStatList->Stats;
	const int nIndex = STATLIST_FindStatIndex_6FDB6300(pArray, nPackedStat);
	D2StatStrc* pStat = nIndex >= 0 ? &pArray->pStat[nIndex] : nullptr;
	if (!pStat)
		pStat = STATLIST_InsertStatOrFail_6FDB6970(pStatList->pMemPool, pArray, nPackedStat);
	if (!pStat)
		return 0;

	pStat->nValue = static_cast<int32_t>(static_cast<uint32_t>(pStat->nValue) + static_cast<uint32_t>(nDelta));
	if (pStat->nValue == 0)
		STATLIST_RemoveStat_6FDB6A30(pStatList->pMemPool, pArray, pStat);

	sub_6FDB6C10(static_cast<D2StatListExStrc*>(pStatList), nPackedStat, nDelta, nullptr);

	if (static_cast<int32_t>(pStatList->dwFlags) < 0 && pStatList->dwOwnerType == 0)
		STATLIST_InsertStatModOrFail_6FDB7690(pStatList, nPackedStat);
	return 1;
}

extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_3a330(uint32_t pStatList, uint32_t nPackedStat, int32_t nDelta)
{
	D2Common_3A330_AddStat(reinterpret_cast<D2StatListStrc*>(static_cast<uintptr_t>(pStatList)), nPackedStat, nDelta);
}

// 1.13c D2Common+0x30AC0 (#10246): the skills.txt record of skill nSkillId inside the unit's skill list.
// The row is pSkillsTxt + nSkillId * 0x23C; an out-of-range id or a null row is fatal in retail (0xDFB),
// a null unit is fatal 0x638, both modelled as a null return. The list lives at unit+0xA8: its +0x04 is
// the first node, each node {record at +0x00, next at +0x04}. Returns the node whose record matches.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_30ac0(uint32_t pUnit, int32_t nSkillId)
{
	uint8_t* u = P(pUnit);
	if (nSkillId < 0 || !sgptDataTables->pSkillsTxt || nSkillId >= sgptDataTables->nSkillsTxtRecordCount)
		return 0;
	if (!u)
		return 0;

	const uint32_t pRow = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&sgptDataTables->pSkillsTxt[nSkillId]));
	const uint32_t pList = W(u, 0xA8);
	uint32_t pNode = pList ? W(P(pList), 4) : 0;
	while (pNode)
	{
		if (W(P(pNode), 0) == pRow)
			return pNode;
		pNode = W(P(pNode), 4);
	}
	return 0;
}

// 1.13c D2Common+0x5A810 (#10249): string from a unit's seed roll.
// A null unit gives the static empty string. With a positive global roll-table count, 0x51100 rolls on
// the unit's seed (+0x20); a non-zero ushort entry at that index is translated through D2Lang 10003 and
// returned when non-null. Otherwise the static empty string (retail 0x6FDF1484) is returned; our image
// has no such object, so the empty case is a null pointer. The table globals and both callees are env
// seams.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_5a810(uint32_t pUnit,
	int32_t (*pfnRoll51100)(uint32_t),
	int32_t (*pfnTableEntry)(uint32_t),
	uint32_t (*pfnLang10003)(uint32_t))
{
	uint8_t* u = P(pUnit);
	if (!u || !pfnRoll51100 || !pfnTableEntry)
		return 0;

	const int32_t nRoll = pfnRoll51100(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(u + 0x20)));
	const int32_t nStringId = pfnTableEntry(static_cast<uint32_t>(nRoll));
	if (nStringId == 0)
		return 0;
	return pfnLang10003 ? pfnLang10003(static_cast<uint32_t>(nStringId)) : 0;
}

// 1.13c D2Common+0x46770 (#10260): assign the rune properties of an item.
// 0x26BA0 gives the runes.txt row; none means 0. When the item's stat list (item+0x5C) has a negative
// dword at +0x10, both node lists (+0x3C and +0x40, next at +0x2C) are scanned and a node whose dword
// at +0x14 is 0xAB aborts with 0. Otherwise a type-4 item with item data (+0x14) gets 0x4000000 in the
// data dword at +0x18, then the row's seven properties (stride 0x10 from +0xB0) are walked: a negative
// property id ends the walk with 1, otherwise 0x45990(6, item, nArg2) is called. Always ends 1.
// 0x26BA0 and 0x45990 (whose other operands are register/stack and dropped) are env seams.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_46770(uint32_t pItem, uint32_t nArg2,
	uint32_t (*pfnRunes26BA0)(uint32_t),
	void (*pfnAssign45990)(int32_t, uint32_t, uint32_t))
{
	uint8_t* item = P(pItem);
	const uint32_t pRunes = (item && pfnRunes26BA0) ? pfnRunes26BA0(pItem) : 0;
	if (pRunes == 0)
		return 0;

	const uint32_t nStatList = W(item, 0x5C);
	if (nStatList && static_cast<int32_t>(W(P(nStatList), 0x10)) < 0)
	{
		for (int nList = 0; nList < 2; ++nList)
		{
			for (uint32_t pNode = W(P(nStatList), 0x3C + nList * 4); pNode != 0; pNode = W(P(pNode), 0x2C))
			{
				if (W(P(pNode), 0x14) == 0xAB)
					return 0;
			}
		}
	}

	const uint32_t pItemData = W(item, 0x14);
	if (W(item, 0) == 4 && pItemData)
		W(P(pItemData), 0x18) |= 0x4000000;

	for (int i = 0; i < 7; ++i)
	{
		const int32_t nProperty = static_cast<int32_t>(W(P(pRunes), 0xB0 + i * 0x10));
		if (nProperty < 0)
			return 1;
		if (pfnAssign45990)
			pfnAssign45990(6, pItem, nArg2);
	}
	return 1;
}

// 1.13c D2Common+0x2F6A0 (#10265): take and reset a type-0 unit's +0x14 data block fields.
// Dword +0x84 and +0x8C are read out, then +0x74 is cleared, +0x7C and +0x90 become -1 and +0x94 is
// cleared. A non-type-0 (or null) unit writes 0 and -1. A type-0 unit with a null data block is fatal in
// retail (0x1470), modelled as a return that leaves the outputs untouched.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_2f6a0(uint32_t pUnit, uint32_t* pOut84, uint32_t* pOut8C)
{
	uint8_t* u = P(pUnit);
	if (u && W(u, 0) == 0)
	{
		const uint32_t pData = W(u, 0x14);
		if (!pData)
			return;
		uint8_t* d = P(pData);
		if (pOut84)
			*pOut84 = W(d, 0x84);
		if (pOut8C)
			*pOut8C = W(d, 0x8C);
		W(d, 0x74) = 0;
		W(d, 0x7C) = 0xFFFFFFFFu;
		W(d, 0x90) = 0xFFFFFFFFu;
		W(d, 0x94) = 0;
		return;
	}

	if (pOut84)
		*pOut84 = 0;
	if (pOut8C)
		*pOut8C = 0xFFFFFFFFu;
}

// 1.13c D2Common+0x5F260 (#10274): unit to display string in pBuffer.
// The buffer's first byte is cleared. A null unit becomes "NULL Unit"; otherwise unit type 0, 1, 2 and 4
// dispatch to 0x5E4D0, 0x5E480, 0x5D800 and 0x5E410 (the first two with the unit type, the buffer cases
// with class id), type 3 formats "missile %d" with the class id, and any other type writes
// "Invalid Unit". Returns pBuffer. Retail reaches the four helpers with register/stack operands the
// decompiler drops, so they are env seams.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_5f260(uint32_t pUnit, uint32_t pBuffer,
	void (*pfn5E4D0)(uint32_t, uint32_t),
	void (*pfn5E480)(uint32_t, uint32_t),
	void (*pfn5D800)(uint32_t, uint32_t),
	void (*pfn5E410)(uint32_t))
{
	uint8_t* u = P(pUnit);
	uint8_t* buf = P(pBuffer);
	if (buf)
		buf[0] = 0;

	if (!u)
	{
		if (buf)
			std::strcpy(reinterpret_cast<char*>(buf), "NULL Unit");
		return pBuffer;
	}

	switch (W(u, 0))
	{
	case 0:
		if (pfn5E4D0)
			pfn5E4D0(W(u, 4), pBuffer);
		break;
	case 1:
		if (pfn5E480)
			pfn5E480(W(u, 0), W(u, 4));
		break;
	case 2:
		if (pfn5D800)
			pfn5D800(W(u, 4), pBuffer);
		break;
	case 3:
		if (buf)
			std::sprintf(reinterpret_cast<char*>(buf), "missile %d", static_cast<int>(W(u, 4)));
		break;
	case 4:
		if (pfn5E410)
			pfn5E410(W(u, 4));
		break;
	default:
		if (buf)
			std::strcpy(reinterpret_cast<char*>(buf), "Invalid Unit");
		break;
	}
	return pBuffer;
}

// 1.13c D2Common+0x12140 (#10301): walk the owner's +0x74 unit list through a callback.
// Each node is offered to the callback (the node is register-only, so the seam takes it); a non-zero
// answer stores the node in pOutArray[nCount++], and count 15 is fatal in retail (0x78), modelled as a
// return there. The next node is at +0xE8. A null callback is fatal 0x72 in retail and returns 0 here.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_12140(uint32_t pOwner, uint32_t pOutArray,
	int32_t (*pfnCallback)(uint32_t))
{
	uint8_t* owner = P(pOwner);
	if (!owner || !pfnCallback)
		return 0;

	uint32_t pNode = W(owner, 0x74);
	int32_t nCount = 0;
	for (;;)
	{
		if (pNode == 0)
			return nCount;
		if (pfnCallback(pNode) != 0)
		{
			if (pOutArray)
				W(P(pOutArray), nCount * 4) = pNode;
			++nCount;
			if (nCount > 0xE)
				return nCount;
		}
		pNode = W(P(pNode), 0xE8);
	}
}

// 1.13c D2Common+0x4E2B0 (#10304): the dword at pObject+0x34. A null object is fatal in retail
// (0x638); modelled as a 0 return.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_4E2B0_GetField34(uint32_t pObject)
{
	uint8_t* o = P(pObject);
	if (!o)
		return 0;
	return W(o, 0x34);
}

// 1.13c D2Common+0x84C30 (#10305): byte nIndex (< 0x10) of a type-1 unit's player data block, which
// 0x13B0 resolves (register-only operand, so it is an env seam). Any other unit type, index, or a null
// block gives 0. The byte is at +0x15 + nIndex.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_84c30(uint32_t pUnit, uint32_t nIndex,
	uint32_t (*pfnBlock13B0)(uint32_t))
{
	uint8_t* u = P(pUnit);
	if (!u || W(u, 0) != 1)
		return 0;
	if (nIndex < 0x10 && pfnBlock13B0)
	{
		const uint32_t pData = pfnBlock13B0(pUnit);
		if (pData)
			return P(pData)[0x15 + nIndex];
	}
	return 0;
}
