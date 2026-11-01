// 1.13c D2Common implementation batch (rows 0x343E0, 0x18280, 0x50150, 0x260F0, 0x37A20, 0x21420).
// Bodies follow the cached 1.13c decompiles (build-wasm/ghidra-d2common/decomp/0x<RVA>.c).
// Callees whose bodies are not recovered are env-seam function pointers after the retail arguments;
// a null seam contributes 0 and the body never faults on it. Static tables embedded in the retail
// image are seam data pointers for the same reason. Fatal paths that end in _exit(-1) are modelled as
// returns, as in the rest of src/core/d2common.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#include "D2DataTbls.h"

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& W(uint8_t* b, int32_t o) { return *reinterpret_cast<uint32_t*>(b + o); }
	inline uint32_t R(const uint8_t* b, int32_t o) { return *reinterpret_cast<const uint32_t*>(b + o); }
	inline uint8_t* T() { return reinterpret_cast<uint8_t*>(sgptDataTables); }
}

// 1.13c D2Common+0x343E0 (#10446): bit nIndex of the second bit array of a unit's stat list.
// Guards: nIndex in [0, datas+0xC4 count), a null unit or stat list or second array yields 0, and the
// stat list header +0x10 must be negative. The array base is +0x58, advanced past ceil(count/32)
// dwords (the first bit array); the bit is taken with the standard 1 << (nIndex & 31) mask.
// 1.13c D2Common+0x343E0
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_StatListTestFlagBit(uint32_t pUnit, uint32_t nIndex)
{
	uint8_t* t = T();
	const int32_t nCount = static_cast<int32_t>(R(t, 0xC4));
	if (static_cast<int32_t>(nIndex) < 0 || static_cast<int32_t>(nIndex) >= nCount)
		return 0;

	uint8_t* u = P(pUnit);
	if (!u)
		return 0;
	const uint32_t pStatList = W(u, 0x5C);
	if (!pStatList)
		return 0;

	uint8_t* s = P(pStatList);
	if (static_cast<int32_t>(W(s, 0x10)) >= 0)
		return 0;

	const uint32_t pBits = W(s, 0x58);
	if (!pBits)
		return 0;

	const int32_t nWords = (nCount + 0x1F) / 0x20;
	const uint8_t* pWords = reinterpret_cast<const uint8_t*>(P(pBits)) + nWords * 4;
	return (1u << (nIndex & 0x1F)) & R(pWords, static_cast<int32_t>(nIndex >> 5) * 4);
}

// 1.13c D2Common+0x18280 (#10454): build a chat message node for szText.
// Empty text gives null. The copy length is the text length capped at 0xFE. The node is 0x7D + 8*n
// bytes; +0x00 is that size, +0x04 the size plus the dropped frame operand, +0x08 a random byte from
// 0x10009, +0x0C zero, +0x10 the text and +0x11+n a terminator. 0x10045 (allocation) and 0x10009 are
// env seams; their retail debug file/line operands are not reproduced.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_ChatNodeNew_18280(uint32_t nFrameAddend, const char* szText,
	void* (*pfnAlloc10045)(const char*, int32_t, int32_t),
	uint32_t (*pfn10009)(void))
{
	if (!szText)
		return 0;

	const size_t nLen = std::strlen(szText);
	if (nLen == 0)
		return 0;
	const int32_t nCopy = nLen < 0xFF ? static_cast<int32_t>(nLen) : 0xFE;

	if (!pfnAlloc10045)
		return 0;
	uint8_t* p = reinterpret_cast<uint8_t*>(pfnAlloc10045(nullptr, 0, 0));
	if (!p)
		return 0;

	const uint32_t nSize = static_cast<uint32_t>(nCopy) * 8 + 0x7D;
	W(p, 0x00) = nSize;
	W(p, 0x04) = nSize + nFrameAddend;
	W(p, 0x0C) = 0;
	std::strncpy(reinterpret_cast<char*>(p + 0x10), szText, static_cast<size_t>(nCopy) + 1);
	p[0x11 + nCopy] = 0;
	p[0x08] = pfn10009 ? static_cast<uint8_t>(pfn10009()) : 0;
	return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p));
}

// 1.13c D2Common+0x50150 (#10444): monster-unit predicate.
// Unit type must be 1 and unit+0x10 must be 0xC, or 0 with nAllowZero set. Callee 0x33C20 is tested
// against the pointer at datas+0x150 and must return 0. Callee 0x1470 (hidden ECX is the data tables,
// its retail first operand is dropped) must return non-zero. The class id (+0x04) below the data
// tables' MonStats count (+0xA80) gives the row at +0xA78, whose short +0x32 is tested. An out-of-range
// class id reaches a decompiler artefact (a short read at absolute 0x32); it yields 0 here.
extern "C"
// 1.13c D2Common+0x50150
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_50150(uint32_t pUnit, int32_t nAllowZero,
	int32_t (*pfn33C20)(uint32_t),
	uint32_t (*pfn1470)(uint32_t, int32_t))
{
	uint8_t* u = P(pUnit);
	if (!u || W(u, 0) != 1)
		return 0;

	const uint32_t nData = W(u, 0x10);
	if (!(nData == 0xC || (nAllowZero != 0 && nData == 0)))
		return 0;

	uint8_t* t = T();
	if (pfn33C20 && pfn33C20(R(t, 0x150)) != 0)
		return 0;

	const int32_t nIndex = static_cast<int32_t>(W(u, 0x04));
	const uint32_t nHiddenEcx = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(sgptDataTables));
	if (!pfn1470 || pfn1470(nHiddenEcx, 7) == 0)
		return 0;

	const int32_t nCount = static_cast<int32_t>(R(t, 0xA80));
	if (nIndex < 0 || nIndex >= nCount)
		return 0;
	const uint32_t pRows = R(t, 0xA78);
	if (!pRows)
		return 0;
	return *reinterpret_cast<const int16_t*>(P(pRows) + nIndex * 0x1A8 + 0x32) != 0 ? 1 : 0;
}

// 1.13c D2Common+0x260F0 D2SetStatWithNoTest (#10463): item-unit value capped at 0x1FF.
// Item unit only (type 4). The base value is 0x22B70(class id). When the stat list (+0x5C) exists and
// the data tables' +0xBD4 dword is at least 0xFF and the pointer at +0xBCC plus 0x14178 is non-null,
// 0x38A80 adds a total for that stat list. A null unit (retail fatal 0xD5B) and a non-item unit
// (0xD5C) return 0. The 0x38A80 operand the decompiler drops is passed as 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t D2SetStatWithNoTest(uint32_t pUnit,
	int32_t (*pfn22B70)(uint32_t),
	int32_t (*pfn38A80)(uint32_t, uint32_t))
{
	uint8_t* u = P(pUnit);
	if (!u)
		return 0;
	if (W(u, 0) != 4)
		return 0;

	int32_t nValue = pfn22B70 ? pfn22B70(W(u, 0x04)) : 0;
	uint8_t* t = T();
	if (W(u, 0x5C) != 0 && static_cast<int32_t>(R(t, 0xBD4)) >= 0xFF)
	{
		const uint32_t pStatList = R(t, 0xBCC) + 0x14178;
		if (pStatList != 0 && pfn38A80)
			nValue += pfn38A80(pStatList, 0);
	}

	if (nValue > 0x1FE)
		nValue = 0x1FF;
	return nValue;
}

// 1.13c D2Common+0x37A20 (#10447): advance a 3-dword state record by one of six table entries.
// param_1+0x04 points at the record. Its +0x00 counter cycles 0..5; +0x30 selects one of two retail
// 6x3-dword tables (pTable0 / pTable1) and the chosen entry's +0x04 goes to record +0x04 while
// record +0x28 times the entry's +0x00 goes to record +0x08. When param_2 is given its room chain
// (+0x10 -> +0x58 -> +0x1D0) yields a value tested against the retail 6-dword threshold table; the
// first index with value < threshold is passed to 0x371C0. 0x376F0 and 0x371C0 are env seams (the
// latter takes the data tables as its hidden ECX), and the three retail tables are seam data pointers.
// A null param_1 is retail's fatal 0x547: _exit(-1).
extern "C" void D2Common_1B0D_Exit(int32_t);
extern "C"
EMSCRIPTEN_KEEPALIVE void D2Common_37A20_AdvanceEnvState(uint32_t pUnit, uint32_t pOther,
	const int32_t* pTable0, const int32_t* pTable1, const int32_t* pThresholds,
	void (*pfn376F0)(int32_t),
	void (*pfn371C0)(uint32_t, uint32_t, uint32_t))
{
	uint8_t* u = P(pUnit);
	if (!u)
		D2Common_1B0D_Exit(-1);
	const uint32_t pState = W(u, 0x04);
	if (!pState)
		return;

	uint8_t* s = P(pState);
	int32_t nCounter = static_cast<int32_t>(W(s, 0x00)) + 1;
	if (nCounter > 5)
		nCounter = 0;
	W(s, 0x00) = static_cast<uint32_t>(nCounter);

	const int32_t* pEntry = nullptr;
	if (W(s, 0x30) == 0)
	{
		if (pTable0)
			pEntry = pTable0 + nCounter * 3;
	}
	else if (pTable1)
	{
		pEntry = pTable1 + nCounter * 3;
	}
	if (!pEntry)
		return;

	W(s, 0x04) = static_cast<uint32_t>(pEntry[1]);
	W(s, 0x08) = W(s, 0x28) * static_cast<uint32_t>(pEntry[0]);

	int32_t nValue = 0;
	uint8_t* o = P(pOther);
	if (o)
	{
		const uint32_t pData = W(o, 0x10);
		if (pData)
		{
			const uint32_t pRoom = W(P(pData), 0x58);
			if (pRoom)
				nValue = static_cast<int32_t>(W(P(pRoom), 0x1D0));
		}
	}

	uint32_t nIndex = 0;
	for (int32_t i = 1; i < 6; ++i)
	{
		if (nValue < (pThresholds ? pThresholds[i] : 0))
		{
			nIndex = static_cast<uint32_t>(static_cast<uint8_t>(i - 1));
			break;
		}
	}

	if (pfn376F0)
		pfn376F0(nValue);
	if (pfn371C0)
		pfn371C0(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(sgptDataTables)), nIndex, pState);

	if (nValue == 0x78)
	{
		s[0x18] = 0xF5;
		s[0x19] = 0xF0;
		s[0x1A] = 0xFF;
	}
}

// 1.13c D2Common+0x21420 (#10457): walk the node list at pUnit+0x60 (head magic 0x1020304) and
// refresh item nodes (node type 4, node +0x10 == 1) from their items.txt row (stride 0x1A8).
// Node data +0x44 selects the path: 1/4/5 test the row's wType[0] (+0x11E) against the data tables'
// type count (+0xBFC) and a per-type mask (base +0xC04, stride +0xC00, bit 11) and call 0x21300 when
// the type is out of range, there are fewer than 0x4C types, or the mask bit is clear. Kind 3 runs
// 0x1E420 for a context, then for indices 1,2,3,4,8,9 calls 0x43710(index, context): a non-null
// ItemTypesTxt result is required (retail fatal 0x150), and either the class id is 0 or 0x1FB50
// returns 0, writing 0xFF into two caller arrays at +pArg2 and +pArg3, or else 0x251C0 decides and the
// +pArg3 entry becomes the roll or is bumped. The walk then follows node data +0x64, stopping on a
// non-type-4 node or null/0xFFFFFFA4 data. Class ids outside the items table are retail fatal 0x1447.
// 0x1E420, 0x21300, 0x1FB50, 0x251C0 and 0x43710 are env seams.
extern "C"
EMSCRIPTEN_KEEPALIVE void D2Common_21420_RefreshItemNodes(uint32_t pUnit, uint32_t nArg2, uint32_t nArg3,
	void (*pfn1E420)(uint8_t*),
	void (*pfn21300)(uint32_t, uint32_t, uint32_t, uint32_t),
	uint32_t (*pfn1FB50)(void),
	int32_t (*pfn251C0)(uint32_t, uint32_t, uint8_t*, int32_t),
	uint8_t (*pfn43710)(uint8_t, uint8_t*))
{
	uint8_t* u = P(pUnit);
	if (!u)
		return;
	const uint32_t pHead = W(u, 0x60);
	if (!pHead)
		return;
	if (W(P(pHead), 0x00) != 0x1020304)
		return;

	uint32_t pNode = W(P(pHead), 0x0C);
	while (pNode != 0)
	{
		uint8_t* t = T();
		uint8_t* n = P(pNode);
		if (W(n, 0x00) == 4 && W(n, 0x10) == 1)
		{
			const uint32_t nItemClass = W(n, 0x04);
			const int32_t nItemCount = sgptDataTables->pItemDataTables.nItemsTxtRecordCount;
			if (nItemCount <= 0 || nItemClass >= static_cast<uint32_t>(nItemCount))
				return;
			if (!sgptDataTables->pItemDataTables.pItemsTxt)
				return;

			const uint32_t pItemData = W(n, 0x14);
			if (pItemData)
			{
				const int8_t nKind = *reinterpret_cast<const int8_t*>(P(pItemData) + 0x44);
				if (nKind == 1 || nKind == 4 || nKind == 5)
				{
					const int32_t nType = *reinterpret_cast<const int16_t*>(
						reinterpret_cast<const uint8_t*>(&sgptDataTables->pItemDataTables.pItemsTxt[nItemClass]) + 0x11E);
					const int32_t nTypeCount = static_cast<int32_t>(R(t, 0xBFC));
					bool bRefresh;
					if (nType < 0 || nTypeCount <= nType || nTypeCount < 0x4C)
					{
						bRefresh = true;
					}
					else
					{
						const uint32_t pMasks = R(t, 0xC04);
						const int32_t nOffset = 8 + static_cast<int32_t>(R(t, 0xC00)) * nType * 4;
						const uint32_t nMask = pMasks ? R(P(pMasks), nOffset) : 0;
						bRefresh = (nMask & 0x800u) == 0;
					}
					if (bRefresh && pfn21300)
						pfn21300(pUnit, nArg2, nArg3, pNode);
				}
				else if (nKind == 3)
				{
					uint8_t local[8] = {0, 0, 0, 0, 0, 0, 0, 0};
					if (pfn1E420)
						pfn1E420(local);
					for (uint32_t i = 0; i < 0x10; ++i)
					{
						if (i == 1 || i == 2 || i == 3 || i == 4 || i == 8 || i == 9)
						{
							const uint8_t bType =
								pfn43710 ? pfn43710(static_cast<uint8_t>(i), local) : 0;
							const uint32_t pItemTypes = static_cast<uint32_t>(
								reinterpret_cast<uintptr_t>(sgptDataTables->pItemTypesTxt));
							if (static_cast<uint32_t>(bType) * 0x34 + pItemTypes == 0)
								return;

							uint32_t nRoll = 0;
							bool bUseRoll = false;
							if (static_cast<int32_t>(nItemClass) >= 1)
							{
								nRoll = pfn1FB50 ? pfn1FB50() : 0;
								bUseRoll = nRoll != 0;
							}
							uint8_t* pThird = P(i + nArg3);
							uint8_t* pSecond = P(i + nArg2);
							if (!bUseRoll)
							{
								*pSecond = 0xFF;
								*pThird = 0xFF;
							}
							else
							{
								*pSecond = static_cast<uint8_t>(nRoll);
								const int32_t nResult =
									pfn251C0 ? pfn251C0(pUnit, pNode, pThird, 0) : 0;
								if (nResult == 0)
									*pThird = 0xFF;
								else
									*pThird = static_cast<uint8_t>(*pThird + 1);
							}
						}
					}
				}
			}
		}

		if (W(n, 0x00) != 4)
			return;
		const uint32_t pNodeData = W(n, 0x14);
		if (pNodeData == 0 || pNodeData == 0xFFFFFFA4u)
			return;
		pNode = R(P(pNodeData), 0x64);
	}
}
