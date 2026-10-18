// 1.13c D2Common implementation batch: +0x37CA0, +0x317C0, +0x3A650, +0x3BC10, +0x30970,
// +0x37920, +0x26AD0, +0x26080, +0x4E150, +0x39CC0, +0x39B60, +0x664B0.
// Naming implementations translated from the pinned decompiles. Retail pointers are four bytes
// wide in the offsets below; guest-side layouts with no vendored twin are read as raw 32-bit
// image addresses, as in the other d2common batch files. Callees that are separate still-UNMAPPED
// rows are env-seam function pointers (null until the environment installs them); the fatal
// paths (Fog message + _exit(-1)) are modelled as returns.
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "DataTbls/ItemsTbls.h"

static_assert(offsetof(D2ItemStatCostTxt, nOp) == 0x54, "itemstatcost.txt nOp");
static_assert(offsetof(D2ItemStatCostTxt, wOpStat) == 0x58, "itemstatcost.txt wOpStat");
static_assert(sizeof(D2ItemStatCostTxt) == 0x144, "itemstatcost.txt stride");

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t A(const void* p) { return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p)); }
	inline uint32_t& W(uint8_t* b, int o) { return *reinterpret_cast<uint32_t*>(b + o); }
	inline int16_t RdI16(const uint8_t* b, int o) { return *reinterpret_cast<const int16_t*>(b + o); }
}

// Env seams for callees that are still-UNMAPPED rows. Signatures follow the 1.13c calls; a null
// pointer means the environment has not installed the implementation yet.
extern "C"
{
	uint32_t (*g_d2common_fn_2D2C0)(int32_t) = nullptr;               // 0x2D2C0
	void (*g_d2common_fn_376F0)(int32_t) = nullptr;                   // 0x376F0
	void (*g_d2common_fn_37170)(void) = nullptr;                      // 0x37170
	void (*g_d2common_fn_371C0)(uint32_t, uint8_t, void*) = nullptr;  // 0x371C0
	uint32_t (*g_d2common_fn_51470)(void*, int32_t) = nullptr;        // 0x1470
	void (*g_d2common_fn_381A0)(uint32_t, void*) = nullptr;           // 0x381A0
	void (*g_d2common_fn_39F70)(int32_t, int32_t) = nullptr;          // 0x39F70
	void (*g_d2common_fn_38550)(void) = nullptr;                      // 0x38550
	void (*g_d2common_fn_92A0)(void*, const void*, uint32_t) = nullptr; // 0x92A0
	int32_t (*g_d2common_fn_230A0)(int32_t) = nullptr;                // 0x230A0
	int32_t (*g_d2common_fn_23290)(void*) = nullptr;                  // 0x23290
	int32_t (*g_d2common_fn_241A0)(void) = nullptr;                   // 0x241A0
	uint32_t (*g_d2common_fn_24610)(void*) = nullptr;                 // 0x24610
	int32_t (*g_d2common_fn_38A80)(void*) = nullptr;                  // 0x38A80
	void (*g_d2common_fn_39380)(void) = nullptr;                      // 0x39380
	int32_t (*g_d2common_fn_39530)(void*, uint32_t) = nullptr;        // 0x39530
	void (*g_d2common_fn_38940)(uint32_t, int32_t, void*, void*) = nullptr; // 0x38940
}

// Retail globals this batch reads. They are initialised by the module's table loader; zero is the
// only value available until the rows that fill them are mapped.
extern "C"
{
	uint32_t g_d2common_dat_6fdeb1f4 = 0;
	uint32_t g_d2common_dat_6fdeb23c = 0;
	uint32_t g_d2common_dat_6fdeb160[18] = {};
	uint32_t g_d2common_dat_6fdeb1f0[18] = {};
	uint32_t g_d2common_dat_6fde9c58 = 0;
	uint16_t g_d2common_dat_6fde9c5a = 0;
	int32_t* g_d2common_dat_6fdf1420 = nullptr;
	int32_t g_d2common_dat_6fdf1424 = 0;
}

extern "C" int32_t d2_fn_d2common_26080(int32_t* pUnit);

// 1.13c D2Common+0x37CA0 (#10337): reset one act record of a unit's path/game block.
// unit+4 is the record. Fields +0x00 and +0x08 are cleared, +0x04 and +0x28 take retail globals,
// +0x2C and +0x30 become 1, then the four 0x2D2C0/0x376F0/0x37170/0x371C0 helpers run with 0.
// Null unit is fatal 0x547; we return.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_37ca0(uint32_t pUnit)
{
	uint8_t* unit = P(pUnit);
	if (!unit)
		return;
	uint8_t* rec = P(W(unit, 0x04));
	if (!rec)
		return;

	W(rec, 0x2C) = 1;
	W(rec, 0x28) = g_d2common_dat_6fdeb23c;
	W(rec, 0x00) = 0;
	W(rec, 0x08) = 0;
	W(rec, 0x04) = g_d2common_dat_6fdeb1f4;

	const uint32_t nResult = g_d2common_fn_2D2C0 ? g_d2common_fn_2D2C0(0) : 0;
	if (g_d2common_fn_376F0)
		g_d2common_fn_376F0(0);
	W(rec, 0x30) = 1;
	if (g_d2common_fn_37170)
		g_d2common_fn_37170();
	if (g_d2common_fn_376F0)
		g_d2common_fn_376F0(0);
	if (g_d2common_fn_371C0)
		g_d2common_fn_371C0(0, static_cast<uint8_t>(nResult & 0xFF), rec);
}

// 1.13c D2Common+0x317C0 (#10338): true when the record's first dword is 1 and the this-side
// query 0x1470(this, 0x0C) answers non-zero. The this argument is otherwise unused.
extern "C"
EMSCRIPTEN_KEEPALIVE bool d2_fn_d2common_317c0(void* pThis, int32_t* pRecord)
{
	if (pRecord && *pRecord == 1 && g_d2common_fn_51470)
		return g_d2common_fn_51470(pThis, 0x0C) != 0;
	return false;
}

// 1.13c D2Common+0x3A650 (#10343): drop entries from a sorted list.
// list+0x24 is the array of 8-byte entries (key dword at +0, value dword at +4) and list+0x28 a
// signed 16-bit count. Each pass binary-searches for the first entry whose key equals the key of
// entry 0; on a hit with a non-zero value it calls 0x381A0(list+0, entry), 0x39F70(-value, 0) and,
// when list+0x10 is negative and list+0x08 is zero, 0x38550. The callees are expected to change
// the count; the pass repeats while the count stays positive. Null list is ignored.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_3a650(uint32_t pList)
{
	uint8_t* list = P(pList);
	if (!list)
		return;

	uint8_t* entries = P(W(list, 0x24));
	if (!entries)
		return;

	int16_t nCount = RdI16(list, 0x28);
	while (nCount > 0)
	{
		int32_t hi = nCount;
		int32_t lo = 0;
		if (hi <= 0)
			break;

		int32_t mid = 0;
		do
		{
			mid = (hi - lo) / 2 + lo;
			const int32_t nKey = *reinterpret_cast<int32_t*>(entries + mid * 8);
			const int32_t nFirstKey = *reinterpret_cast<int32_t*>(entries);

			if (nKey < nFirstKey)
			{
				lo = mid + 1;
				mid = hi;
			}
			else if (nKey <= nFirstKey)
			{
				uint8_t* entry = entries + mid * 8;
				const int32_t nValue = *reinterpret_cast<int32_t*>(entry + 4);
				if (mid >= 0 && static_cast<uint32_t>(nValue) != 0)
				{
					if (g_d2common_fn_381A0)
						g_d2common_fn_381A0(W(list, 0), entry);
					if (g_d2common_fn_39F70)
						g_d2common_fn_39F70(-nValue, 0);
					if (static_cast<int32_t>(W(list, 0x10)) < 0 && W(list, 0x08) == 0 &&
					    g_d2common_fn_38550)
						g_d2common_fn_38550();
				}
				break;
			}
			hi = mid;
		} while (lo < mid);

		nCount = RdI16(list, 0x28);
	}
}

// 1.13c D2Common+0x3BC10 (#10346): toggle bit 22 of the dword at *(unit+0x10)+0x28.
// A non-zero second argument clears the bit, zero sets it.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_3bc10(uint32_t pUnit, int32_t nClear)
{
	uint8_t* unit = P(pUnit);
	if (!unit)
		return;
	uint8_t* inner = P(W(unit, 0x10));
	if (!inner)
		return;

	if (nClear)
		W(inner, 0x28) &= ~0x00400000u;
	else
		W(inner, 0x28) |= 0x00400000u;
}

// 1.13c D2Common+0x30970 (#10356): allocate the three-difficulty quest record set.
// set+0x14 must be null (otherwise fatal 0xF51); the 0x16C-byte set is zeroed. Three times it
// allocates a 0x60-byte quest record (copied from a zeroed scratch block through 0x92A0), stores
// it at set+0x10/+0x14/+0x18, allocates a 16-byte waypoint block, writes 0x0102 and ORs in the
// global waypoint bit, and stores its address at set+0x1C/+0x20/+0x24. It then scans the global
// quest-flag array for the first entry equal to 1 and writes 1<<index to set+0x2C; set+0x4C is 0.
// A null set is fatal 0xF4F; we return.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_30970(uint32_t pSet)
{
	uint8_t* setPtr = P(pSet);
	if (!setPtr)
		return;
	if (W(setPtr, 0x14) != 0)
		return;

	uint8_t* pSetData = static_cast<uint8_t*>(std::calloc(0x5B, 4));
	if (!pSetData)
		return;
	W(setPtr, 0x14) = A(pSetData);
	pSetData[0] = 0;

	for (int nOffset = 0x1C; nOffset < 0x28; nOffset += 4)
	{
		uint8_t* pQuest = static_cast<uint8_t*>(std::calloc(0x18, 4));
		uint8_t* pScratch = static_cast<uint8_t*>(std::calloc(0x18, 4));
		if (!pQuest || !pScratch)
			return;
		if (g_d2common_fn_92A0)
			g_d2common_fn_92A0(pQuest, pScratch, 0x60);
		W(pSetData, nOffset - 0x0C) = A(pQuest);

		uint8_t* pWaypoint = static_cast<uint8_t*>(std::calloc(4, 4));
		if (!pWaypoint)
			return;
		*reinterpret_cast<uint16_t*>(pWaypoint) = 0x0102;
		uint16_t* pBit = reinterpret_cast<uint16_t*>(pWaypoint + g_d2common_dat_6fde9c58 * 2);
		*pBit |= g_d2common_dat_6fde9c5a;
		W(pSetData, nOffset) = A(pWaypoint);
	}

	int32_t nValue = 0;
	for (int32_t i = 0; i < g_d2common_dat_6fdf1424; ++i)
	{
		if (g_d2common_dat_6fdf1420 && g_d2common_dat_6fdf1420[i] == 1)
		{
			nValue = static_cast<int32_t>(1u << (static_cast<uint32_t>(i) & 0x1F));
			break;
		}
	}
	W(pSetData, 0x2C) = static_cast<uint32_t>(nValue);
	W(pSetData, 0x4C) = 0;
}

// 1.13c D2Common+0x37920 (#10358): set one act/time record of a unit's path/game block.
// unit+4 is the record. nAct must be 0..5 (fatal 0x211), nValue non-negative (fatal 0x212); a
// value beyond record+0x28 * 0x168 wraps to 0. The act and value go to +0x00/+0x08, the record's
// +0x04 takes entry nAct of the table selected by nFlag (DAT_6fdeb160 or DAT_6fdeb1f0, stride
// three dwords, field 1). A non-zero owner contributes its chain +0x10/+0x58/+0x1D0 value, which
// drives 0x2D2C0/0x376F0 and, with nFlag, 0x37170/0x371C0; nFlag lands at +0x30. When that value
// is 0x78 the bytes at +0x18..+0x1A become F5 F0 FF. Null unit is fatal 0x547; we return.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_37920(uint32_t pUnit, uint32_t pOwner, int32_t nAct,
                                               int32_t nValue, int32_t nFlag)
{
	uint8_t* unit = P(pUnit);
	if (!unit)
		return;
	uint8_t* rec = P(W(unit, 0x04));
	if (!rec)
		return;

	if (nAct < 0)
		return;                                     // fatal 0x210
	if (nAct >= 6)
		return;                                     // fatal 0x211
	if (nValue < 0)
		return;                                     // fatal 0x212

	if (static_cast<int32_t>(W(rec, 0x28)) * 0x168 < nValue)
		nValue = 0;
	W(rec, 0x00) = static_cast<uint32_t>(nAct);
	W(rec, 0x08) = static_cast<uint32_t>(nValue);

	const uint32_t* pTable = nFlag == 0 ? g_d2common_dat_6fdeb160 : g_d2common_dat_6fdeb1f0;
	W(rec, 0x04) = pTable[nAct * 3 + 1];

	int32_t nChain = 0;
	if (pOwner)
	{
		uint8_t* pFirst = P(W(P(pOwner), 0x10));
		uint8_t* pSecond = pFirst ? P(W(pFirst, 0x58)) : nullptr;
		if (pSecond)
			nChain = static_cast<int32_t>(W(pSecond, 0x1D0));
	}

	const uint32_t nResult = g_d2common_fn_2D2C0 ? g_d2common_fn_2D2C0(nChain) : 0;
	if (g_d2common_fn_376F0)
		g_d2common_fn_376F0(nChain);
	W(rec, 0x2C) = static_cast<uint32_t>(nFlag);
	if (nFlag != 0)
	{
		if (g_d2common_fn_37170)
			g_d2common_fn_37170();
		if (g_d2common_fn_376F0)
			g_d2common_fn_376F0(nChain);
		if (g_d2common_fn_371C0)
			g_d2common_fn_371C0(0, static_cast<uint8_t>(nResult & 0xFF), rec);
	}
	if (nChain == 0x78)
	{
		rec[0x18] = 0xF5;
		rec[0x19] = 0xF0;
		rec[0x1A] = 0xFF;
	}
}

// 1.13c D2Common+0x26AD0 (#10360): item acceptability predicate.
// A null unit is fatal 0x67E. A type-4 (item) unit whose 0x230A0(classId) answers 4 is rejected.
// Then, unless the item-data flags at unit[5]+0x18 carry 0x1000, an items.txt record with nQuest
// set and a 0x23290 code other than 'leg ' ('gel ' little-endian word 0x2067656C) is rejected.
// The remaining test chains 0x241A0, 0x24610 and 0x26080; the answer is whether 0x26080 returns
// zero. The decompile's leftover EDX is modelled as 0.
extern "C"
EMSCRIPTEN_KEEPALIVE bool d2_fn_d2common_26ad0(int32_t* pUnit)
{
	if (!pUnit)
		return false;

	uint8_t* pFlags = P(static_cast<uint32_t>(pUnit[5]));
	const uint32_t nFlags = pFlags ? W(pFlags, 0x18) : 0;

	if (pUnit[0] == 4 && g_d2common_fn_230A0 && g_d2common_fn_230A0(pUnit[1]) == 4)
		return false;

	if (pUnit[0] != 4 || !pFlags || (nFlags & 0x1000u) == 0)
	{
		const D2ItemsTxt* pRow = DATATBLS_GetItemsTxtRecord(pUnit[1]);
		if (pRow && pRow->nQuest != 0 && g_d2common_fn_23290 &&
		    g_d2common_fn_23290(pUnit) != 0x2067656C)
			return false;

		const bool bNoOwnerFlag = pUnit[0] != 4 || !pFlags || (nFlags & 0x100u) == 0;
		const bool bNoChainFlag = true;   // decompile's leftover EDX != 4; EDX is modelled as 0
		if (bNoOwnerFlag && g_d2common_fn_241A0 && g_d2common_fn_241A0() == 0 && bNoChainFlag &&
		    g_d2common_fn_24610 && static_cast<uint8_t>(g_d2common_fn_24610(pUnit)) != 0)
		{
			return d2_fn_d2common_26080(pUnit) == 0;
		}
	}
	return false;
}

// 1.13c D2Common+0x26080 (#10366): op-stat record of the item's 0x5C field.
// Only a type-4 unit is valid (fatal 0xEA7/0xEA8). When unit[0x5C] is non-zero, itemstatcost has
// more than 0xC2 records and its base is loaded, the answer is 0x38A80(record[0xC2]); otherwise 0.
// Fatal paths return 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_26080(int32_t* pUnit)
{
	if (!pUnit)
		return 0;
	if (pUnit[0] != 4)
		return 0;

	if (pUnit[0x17] != 0 && sgptDataTables && sgptDataTables->pItemStatCostTxt &&
	    sgptDataTables->nItemStatCostTxtRecordCount > 0xC2 && g_d2common_fn_38A80)
		return g_d2common_fn_38A80(&sgptDataTables->pItemStatCostTxt[0xC2]);

	return 0;
}

// 1.13c D2Common+0x4E150 (#10371): the dword at struct+0x10. Null is fatal 0xA24; we answer 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_4e150(uint32_t pStruct)
{
	uint8_t* s = P(pStruct);
	return s ? W(s, 0x10) : 0;
}

// 1.13c D2Common+0x39CC0 (#10379): forwards to 0x39380.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_39cc0(void)
{
	if (g_d2common_fn_39380)
		g_d2common_fn_39380();
}

// 1.13c D2Common+0x39B60 (#10381): apply the op-stat chain of a unit's effect list.
// Both arguments are required; unit+0 has to be 0 or 1 and param+0x5C a pointer whose +0x10 is
// negative. The list at effect+0x48 holds *(int16*)(effect+0x4C) entries of 8 bytes; each entry's
// u16 at +2 indexes itemstatcost. Rows with nOp in 6..7 contribute their wOpStat[3] ids: for each
// id before 0xFFFF the code resolves the op row and calls 0x39530(unitData, id<<16) then
// 0x38940(id<<16, value, opRow, unit).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_39b60(uint16_t* pUnit, uint32_t pParam)
{
	uint8_t* unit = reinterpret_cast<uint8_t*>(pUnit);
	uint8_t* param = P(pParam);
	if (!unit || !param)
		return;
	if (static_cast<int32_t>(W(unit, 0x00)) != 0 && static_cast<int32_t>(W(unit, 0x00)) != 1)
		return;

	uint8_t* effect = P(W(param, 0x5C));
	if (!effect || static_cast<int32_t>(W(effect, 0x10)) >= 0)
		return;

	if (!sgptDataTables || !sgptDataTables->pItemStatCostTxt)
		return;

	const int16_t nCount = RdI16(effect, 0x4C);
	uintptr_t current = static_cast<uintptr_t>(W(effect, 0x48));
	const uintptr_t end = current + static_cast<uintptr_t>(nCount) * 8;
	uint8_t* unitData = P(W(unit, 0x5C));

	const D2ItemStatCostTxt* pBase = sgptDataTables->pItemStatCostTxt;
	const int32_t nStatCost = sgptDataTables->nItemStatCostTxtRecordCount;

	for (; current < end; current += 8)
	{
		const uint8_t* element = reinterpret_cast<const uint8_t*>(current);
		const uint16_t nIndex = *reinterpret_cast<const uint16_t*>(element + 2);
		const D2ItemStatCostTxt* pRow =
			nIndex < nStatCost ? &pBase[nIndex] : nullptr;
		if (!pRow || pRow->nOp <= 5 || pRow->nOp >= 8)
			continue;

		for (int i = 0; i < 3; ++i)
		{
			const uint16_t nStatId = pRow->wOpStat[i];
			if (nStatId == 0xFFFF)
				break;
			const D2ItemStatCostTxt* pOpRow =
				nStatId < nStatCost ? &pBase[nStatId] : nullptr;
			const uint32_t nShifted = static_cast<uint32_t>(nStatId) << 16;
			const int32_t nValue =
				g_d2common_fn_39530 ? g_d2common_fn_39530(unitData, nShifted) : 0;
			if (g_d2common_fn_38940)
				g_d2common_fn_38940(nShifted, nValue,
				                    const_cast<D2ItemStatCostTxt*>(pOpRow), unit);
		}
	}
}

// 1.13c D2Common+0x664B0 (#10384): insertion-sort a unit's linked list by effect key.
// struct+0x74 is the head; every node's next is at +0xE8 and its payload pointer at +0x2C. The key
// is payload+0x08 for node types 2 and 4..5 and payload+0x0C otherwise. The list is walked with a
// trailing pointer, swapping a node ahead of a lower-keyed predecessor until a full pass changes
// nothing. Null struct is fatal 0x286 and a broken chain fatal 0x78A; both return.
namespace
{
	int32_t Batch37ca0_NodeKey(uint8_t* pNode)
	{
		const int32_t nType = static_cast<int32_t>(W(pNode, 0x00));
		uint8_t* pData = P(W(pNode, 0x2C));
		if (!pData)
			return 0;
		const bool bFirstHalf = nType == 2 || (nType > 3 && nType < 6);
		return static_cast<int32_t>(W(pData, bFirstHalf ? 0x08 : 0x0C));
	}
}

extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_664b0(uint32_t pStruct)
{
	uint8_t* s = P(pStruct);
	if (!s)
		return;
	uint8_t* pHead = P(W(s, 0x74));
	if (!pHead)
		return;

	uint8_t* pCurrent = P(W(pHead, 0xE8));
	uint8_t* pTrailing = nullptr;

	for (;;)
	{
		if (!pCurrent)
			return;

		bool bChanged = false;
		uint8_t* pA = pCurrent;
		uint8_t* pB = pHead;

		for (;;)
		{
			if (!pA || !pB)
				return;                         // fatal 0x78A
			const int32_t nKeyB = Batch37ca0_NodeKey(pB);
			const int32_t nKeyA = Batch37ca0_NodeKey(pA);

			if (nKeyA < nKeyB)
			{
				if (!pTrailing)
					W(s, 0x74) = A(pA);
				else
					W(pTrailing, 0xE8) = A(pA);
				W(pB, 0xE8) = W(pA, 0xE8);
				W(pA, 0xE8) = A(pB);
				pCurrent = P(W(pB, 0xE8));
				bChanged = true;
				pHead = pB;
				pTrailing = pA;
			}
			else
			{
				pCurrent = P(W(pA, 0xE8));
				pHead = pA;
				pTrailing = pB;
			}

			pA = pCurrent;
			pB = pHead;
			if (!pCurrent)
				break;
		}

		if (!bChanged)
			return;
	}
}
