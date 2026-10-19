// src/core/d2common/d2common_batch_87080.cpp -- six 1.13c D2Common.dll rows transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2common/decomp/0x<RVA>.c). Image base 0x6FD50000.
//
// Retail records with no vendored twin are still read by their retail byte offsets (rd8/rd16/rd32),
// and retail addresses travel as 32-bit integers, as in the other d2common batches. Callees that
// src/ does not implement are function-pointer seams on D2CommonBatch87080Env, handed over after the
// retail arguments; a missing seam contributes nothing. Register-only operands the decompile drops
// (in_EAX / unaff_EBX / unaff_EBL / unaff_EDI) are named parameters. Paths that end in _exit(-1) in
// retail are modelled as plain returns.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Items.h"
#include "D2StatList.h"
#include "DataTbls/ItemsTbls.h"
#include "DataTbls/LevelsTbls.h"
#include "Drlg/D2DrlgActivate.h"
#include "Drlg/D2DrlgDrlg.h"

static_assert(sizeof(D2LevelDefBin) == 0x9C, "levels.bin record stride");
static_assert(offsetof(D2LevelDefBin, nIntensity) == 0x88, "levels.bin nIntensity");
static_assert(offsetof(D2LevelDefBin, nRed) == 0x89, "levels.bin nRed");
static_assert(offsetof(D2LevelDefBin, nGreen) == 0x8A, "levels.bin nGreen");
static_assert(offsetof(D2LevelDefBin, nBlue) == 0x8B, "levels.bin nBlue");
static_assert(offsetof(D2SetItemsTxt, nSetId) == 0x2C, "setitems.txt nSetId");
static_assert(offsetof(D2SetItemsTxt, nAddFunc) == 0x87, "setitems.txt nAddFunc");

namespace
{
	inline const uint8_t* P(uint32_t a) { return reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint8_t*       Pw(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }

	inline uint8_t  rd8 (uint32_t a, uint32_t o) { uint8_t  v; std::memcpy(&v, P(a) + o, 1); return v; }
	inline uint16_t rd16(uint32_t a, uint32_t o) { uint16_t v; std::memcpy(&v, P(a) + o, 2); return v; }
	inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, P(a) + o, 4); return v; }
	inline void     wr8 (uint32_t a, uint32_t o, uint8_t v) { std::memcpy(Pw(a) + o, &v, 1); }
	inline void     wr16(uint32_t a, uint32_t o, uint16_t v) { std::memcpy(Pw(a) + o, &v, 2); }
}

// The 1.10f set tables, defined in src/core/d2moo/src/Items/Items.cpp. 0x6FDDCA08 (item-set states)
// and 0x6FDE4748 (set-item-count to bonus-count) in the 1.13c image.
extern int gnItemSetStates[6];
extern int gnSetMaskToBonusMappingTable[64];

// Implemented in d2common_batch_1610.cpp (1.13c D2Common+0x1800).
D2SetItemsTxt* D2Common_1800_GetSetItemsTxtRecord(int32_t nSetItemId);

// Host seams for the callees the cached decompiles name but src/ does not implement yet.
struct D2CommonBatch87080Env {
	// 0x45F90(unit, state) with the item still in EBX: 1.10f ITEMMODS_UpdateFullSetBoni(unit, item, state).
	void (*sub45F90)(uint32_t pUnit, uint32_t pItem, int32_t nState);
	// 0x7BB70(room, status) with both in registers: unlink the room from its status list and relink
	// it at status when the status changes; a status >= 4 leaves it unlinked.
	void (*sub7BB70)(uint32_t pRoom, uint8_t nStatus);
	// gRoomExSetStatus[status](room): the four callbacks in the table at retail 0x6FDE9BE8.
	void (*setStatus[4])(uint32_t pRoom);
};

// 1.13c D2Common+0x87080: the CRT's signed 64-bit division helper __alldiv. Each operand is a pair
// of 32-bit halves. The routine negates each operand, divides unsigned and negates the quotient when
// exactly one operand was negative. A zero divisor reaches integer division by zero in retail and
// raises the processor's divide-error exception; preserve that trap in the wasm implementation.
extern "C"
EMSCRIPTEN_KEEPALIVE uint64_t d2_fn_d2common_87080(uint32_t nDividendLow, uint32_t nDividendHigh,
                                                   uint32_t nDivisorLow, uint32_t nDivisorHigh)
{
	const uint64_t nDividend = (static_cast<uint64_t>(nDividendHigh) << 32) | nDividendLow;
	const uint64_t nDivisor  = (static_cast<uint64_t>(nDivisorHigh) << 32) | nDivisorLow;
	if (nDivisor == 0)
		__builtin_trap();

	const bool bNegDividend = (nDividendHigh & 0x80000000u) != 0;
	const bool bNegDivisor  = (nDivisorHigh & 0x80000000u) != 0;
	const uint64_t nAbsDividend = bNegDividend ? 0 - nDividend : nDividend;
	const uint64_t nAbsDivisor  = bNegDivisor ? 0 - nDivisor : nDivisor;

	uint64_t nQuotient = nAbsDividend / nAbsDivisor;
	if (bNegDividend != bNegDivisor)
		nQuotient = 0 - nQuotient;
	return nQuotient;
}

// 1.13c D2Common+0x2C630 (1.10f sub_6FDA4490): apply or remove one set item's bonus.
// pItem is the stacked argument, pUnit the register argument; nMode 0 adds, 1 removes. The guard
// requires mode < 2, both units, a set item (quality 5) and a unit that is not itself a magic, set,
// rare, unique, crafted or tempered item. The set-items row's nSetId is matched against the
// STAT_VALUE entry of the six item-set state stat lists; the first empty state is remembered for the
// allocation branch, which allocates on the unit and posts the list before setting its state.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_2c630(uint32_t pUnit, uint32_t pItem, int32_t nMode,
                                                  const D2CommonBatch87080Env* env)
{
	if (nMode >= 2 || pUnit == 0 || pItem == 0)
		return 0;

	D2UnitStrc* pUnitS = reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pUnit));
	D2UnitStrc* pItemS = reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pItem));
	if (pItemS->dwUnitType != 4 || ITEMS_GetItemQuality(pItemS) != 5)
		return 0;
	if (pUnitS->dwUnitType == 4 && ITEMS_IsMagSetRarUniCrfOrTmp(pUnitS) != 0)
		return 0;

	const int32_t nFileIndex = ITEMS_GetFileIndex(pItemS);
	D2SetItemsTxt* pSetItemsTxt = D2Common_1800_GetSetItemsTxtRecord(nFileIndex);
	if (pSetItemsTxt == nullptr)
		return 0;

	const int32_t nSetId = pSetItemsTxt->nSetId;
	if (nSetId < 0 || nSetId >= sgptDataTables->nSetsTxtRecordCount)
		return 0;

	int32_t nEmptyState = -1;
	int32_t nState = 0;
	D2StatListStrc* pStatList = nullptr;
	bool bFound = false;

	for (int32_t i = 0; i < 6; ++i)
	{
		nState = gnItemSetStates[i];
		pStatList = STATLIST_GetStatListFromUnitAndState(pUnitS, nState);
		if (pStatList == nullptr)
		{
			if (nEmptyState < 0)
				nEmptyState = i;
			continue;
		}

		if (nSetId == STATLIST_GetStatValue(pStatList, STAT_VALUE, 0))
		{
			bFound = true;
			if (nMode == 0)
				STATLIST_RemoveAllStats(pStatList);
			break;
		}
	}

	if (!bFound)
	{
		if (nMode == 1 || nEmptyState < 0)
			return 0;

		nState = gnItemSetStates[nEmptyState];
		pStatList = STATLIST_AllocStatList(pUnitS->pMemoryPool, 0, 0,
			static_cast<int>(pItemS->dwUnitType), pItemS->dwUnitId);
		D2COMMON_10475_PostStatToStatList(pUnitS, pStatList, 1);
		if (pStatList != nullptr)
			STATLIST_SetState(pStatList, nState);
	}

	if (nMode == 0)
	{
		STATLIST_AddStat(pStatList, STAT_VALUE, nSetId, 0);
		if (env != nullptr && env->sub45F90 != nullptr)
			env->sub45F90(pUnit, pItem, nState);
	}
	else if (nMode == 1)
	{
		D2Common_ExpireStatList_6FDB6E30(pStatList);
		STATLIST_FreeStatList(pStatList);
		return 1;
	}

	return 1;
}

// 1.13c D2Common+0x2C7C0 (1.10f sub_6FDA4380): toggle the item-set states of a set item from the
// set-item mask. nAddFunc 1 sets or clears each state except the one belonging to the item's own set
// rank (the index skips that state); nAddFunc 2 clears the states below the bonus count, then for the
// remaining states (indices nBonuses-1 .. 4, as the retail table end 0x6FDDCA1C makes five entries)
// finds the owner's extended stat list and re-posts the state's list with STATLIST_SET (0x2000) set
// after expiring it, unless that flag is already present.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_2c7c0(uint32_t pItem, uint32_t nSetItemMask)
{
	if (pItem == 0)
		return 0;

	D2UnitStrc* pItemS = reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pItem));
	if (pItemS->dwUnitType != 4 || ITEMS_GetItemQuality(pItemS) != 5)
		return 0;

	D2SetItemsTxt* pSetItemsTxt = ITEMS_GetSetItemsTxtRecordFromItem(pItemS);
	if (pSetItemsTxt == nullptr || pSetItemsTxt->nAddFunc == 0)
		return 0;

	if (pSetItemsTxt->nAddFunc == 1)
	{
		const int32_t nSetItems = ITEMS_GetNoOfSetItemsFromItem(pItemS);
		for (int32_t i = 0; i < 6; ++i)
		{
			if (i == nSetItems)
				continue;

			int32_t nIndex = i;
			if (nSetItems < i)
				--nIndex;

			const bool bUnset = (nSetItemMask & (1u << i)) == 0;
			D2Common_10574(pItemS, gnItemSetStates[nIndex], bUnset ? 1 : 0);
		}
	}
	else if (pSetItemsTxt->nAddFunc == 2)
	{
		const int32_t nBonuses = nSetItemMask < 64
			? gnSetMaskToBonusMappingTable[nSetItemMask] : 0;
		const int32_t nFirst = nBonuses - 1;

		for (int32_t i = 0; i < nFirst; ++i)
			D2Common_10574(pItemS, gnItemSetStates[i], 0);

		if (nFirst >= 5)
			return 1;

		for (int32_t i = nFirst; i < 5; ++i)
		{
			// With zero bonuses retail starts at DAT_6fddca08 - 4. The preceding word in
			// the 1.13c image is zero, so model it explicitly instead of indexing before the array.
			const int32_t nState = i < 0 ? 0 : gnItemSetStates[i];
			D2StatListExStrc* pStatListEx = pItemS->pStatListEx;
			if (nState == 0 || pStatListEx == nullptr || static_cast<int32_t>(pStatListEx->dwFlags) >= 0)
				continue;

			D2StatListStrc* pStateList = D2Common_GetStateFromStatListEx_6FDB8190(pStatListEx, nState);
			if (pStateList == nullptr || (pStateList->dwFlags & 0x2000u) != 0)
				continue;

			D2Common_ExpireStatList_6FDB6E30(pStateList);
			pStateList->dwFlags |= 0x2000u;
			D2COMMON_10475_PostStatToStatList(pItemS, pStateList, 1);
		}
	}

	return 1;
}

// 1.13c D2Common+0x7BE90 (1.10f DRLGACTIVATE_RoomExIdentifyRealStatus): when the room's current
// status byte (+0x44) is at least 4 or its ref count at +0xC+status*2 is zero, take the first status
// in 0..3 whose ref count is non-zero (4 when none is) and force the room to it through 0x7BB70; the
// status byte is written afterwards.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_7be90(uint32_t pRoom, const D2CommonBatch87080Env* env)
{
	if (pRoom == 0)
		return;

	const uint8_t nStatus = rd8(pRoom, 0x44);
	if (nStatus < 4 && rd16(pRoom, 0xC + static_cast<uint32_t>(nStatus) * 2) != 0)
		return;

	uint8_t nRealStatus = 0;
	do
	{
		if (rd16(pRoom, 0xC + static_cast<uint32_t>(nRealStatus) * 2) != 0)
			break;
		++nRealStatus;
	} while (nRealStatus < 4);

	if (env != nullptr && env->sub7BB70 != nullptr)
		env->sub7BB70(pRoom, nRealStatus);
	wr8(pRoom, 0x44, nRealStatus);
}

// 1.13c D2Common+0x7C090 (1.10f DRLGACTIVATE_RoomSetAndPropagateStatus): raise a room to nStatus.
// nStatus+1 is propagated to the rooms near it first. When the room's status byte (+0x44) is at least
// nStatus and no ref count in 0..nStatus is non-zero, the per-status callback runs; the status ref
// count at +0xC+nStatus*2 is then incremented. The room is the register argument (EDI) and nStatus the
// low byte of BL, as 0x3CC40 and 0x7C300 call it.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_7c090(uint32_t pRoom, uint32_t nStatus,
                                               const D2CommonBatch87080Env* env)
{
	if (pRoom == 0)
		return;

	const uint8_t nBL = static_cast<uint8_t>(nStatus);
	const uint32_t pLevel = rd32(pRoom, 0x58);
	const uint32_t pDrlg = pLevel != 0 ? rd32(pLevel, 0x1B4) : 0;
	const uint32_t pMemPool = pDrlg != 0 ? rd32(pDrlg, 0x478) : 0;

	DRLGACTIVATE_RoomExPropagateSetStatus(
		reinterpret_cast<void*>(static_cast<uintptr_t>(pMemPool)),
		reinterpret_cast<D2DrlgRoomStrc*>(static_cast<uintptr_t>(pRoom)),
		static_cast<uint8_t>(nBL + 1));

	if (nBL <= rd8(pRoom, 0x44))
	{
		int32_t i = 0;
		while (rd16(pRoom, 0xC + static_cast<uint32_t>(i) * 2) == 0)
		{
			++i;
			if (i > static_cast<int32_t>(nBL))
			{
				if (env != nullptr && nBL < 4 && env->setStatus[nBL] != nullptr)
					env->setStatus[nBL](pRoom);
				break;
			}
		}
	}

	wr16(pRoom, 0xC + static_cast<uint32_t>(nBL) * 2,
		static_cast<uint16_t>(rd16(pRoom, 0xC + static_cast<uint32_t>(nBL) * 2) + 1));
}

// 1.13c D2Common+0x44D90: copy a unit's level palette. The level id is at pGfxDataCopy(+0x58)+0x1D0;
// an id outside [0, nLevelsTxtRecordCount) makes retail read the zero-filled low image at 0x88, so the
// outputs are zero. Otherwise the four bytes come from the levels.bin row's nIntensity/nRed/nGreen/
// nBlue. The unit is the register argument (EAX) and the four outputs are the stack arguments. Retail
// dereferences the unit and its gfx data without null guards, so invalid pointers trap rather than
// silently selecting level zero.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_44d90(uint32_t pUnit, uint8_t* pnIntensity, uint8_t* pnRed,
                                               uint8_t* pnGreen, uint8_t* pnBlue)
{
	if (pUnit == 0)
		__builtin_trap();

	const uint32_t pGfx = rd32(pUnit, 0x58);
	if (pGfx == 0)
		__builtin_trap();

	const int32_t nLevelId = static_cast<int32_t>(rd32(pGfx, 0x1D0));

	uint8_t nIntensity = 0;
	uint8_t nRed = 0;
	uint8_t nGreen = 0;
	uint8_t nBlue = 0;

	if (nLevelId >= 0 && nLevelId < sgptDataTables->nLevelsTxtRecordCount)
	{
		const D2LevelDefBin* pRow = &sgptDataTables->pLevelDefBin[nLevelId];
		nIntensity = pRow->nIntensity;
		nRed = pRow->nRed;
		nGreen = pRow->nGreen;
		nBlue = pRow->nBlue;
	}

	*pnIntensity = nIntensity;
	*pnRed = nRed;
	*pnGreen = nGreen;
	*pnBlue = nBlue;
}
