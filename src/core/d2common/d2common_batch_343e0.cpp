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

// 1.13c D2Common+0x18280 (#10454): build a chat message node for szText (stdcall pool, text, addend).
// Empty text gives null. The copy length is the text length capped at 0xFE. The node is a 0x110-byte
// pool allocation (Chat.cpp line 0x32); +0x00 is 0x7D + 8*n, +0x04 that plus nAddend, +0x08 a random
// byte from the Lang random thunk, +0x0C zero, +0x10 the text and +0x11+n a terminator. The allocator
// and the random byte are seams; their retail file/line operands are not reproduced.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_ChatNodeNew_18280(uint32_t pPool, const char* szText, uint32_t nAddend,
	void* (*pfnAlloc)(uint32_t pool, uint32_t size),
	uint32_t (*pfnRand)(void))
{
	if (!szText || !szText[0])
		return 0;
	const size_t nLen = std::strlen(szText);
	const int32_t nCopy = nLen < 0xFF ? static_cast<int32_t>(nLen) : 0xFE;

	uint8_t* p = reinterpret_cast<uint8_t*>(pfnAlloc(pPool, 0x110));
	const uint32_t nSize = static_cast<uint32_t>(nCopy) * 8 + 0x7D;
	W(p, 0x00) = nSize;
	W(p, 0x04) = nSize + nAddend;
	W(p, 0x0C) = 0;
	std::strncpy(reinterpret_cast<char*>(p + 0x10), szText, static_cast<size_t>(nCopy) + 1);
	p[0x11 + nCopy] = 0;
	p[0x08] = static_cast<uint8_t>(pfnRand());
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
	if (W(u, 0x5C) != 0 && sgptDataTables->nItemStatCostTxtRecordCount >= 0xFF)
	{
		const D2ItemStatCostTxt* pRow = sgptDataTables->pItemStatCostTxt + 0xFE;   // retail: +0xBCC table, row 0xFE (+0x14178)
		if (sgptDataTables->pItemStatCostTxt && pfn38A80)
			nValue += pfn38A80(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pRow)), 0);
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
	void (*pfn376F0)(uint32_t ecx, uint32_t edi, void* rec),     // 0x376F0: ECX level, EDI band, ESI rec
	void (*pfn371C0)(uint32_t ecx, uint32_t edx, void* rec))     // 0x371C0: EDX band, stack arg rec
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
		pfn376F0(static_cast<uint32_t>(nValue), nIndex, s);
	if (pfn371C0)
		pfn371C0(static_cast<uint32_t>(nValue), nIndex, s);

	if (nValue == 0x78)
	{
		s[0x18] = 0xF5;
		s[0x19] = 0xF0;
		s[0x1A] = 0xFF;
	}
}
