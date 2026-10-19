// 1.13c D2Common implementation batch: +0x34E70, +0x356E0, +0x37170, +0x38940.
// Bodies translated from the pinned 1.13c decompiles. Structs with a vendored twin are read by name;
// retail room coordinates live at +0x4C and keep their 1.13c offsets. Callees that are still
// UNMAPPED rows are env-seam function pointers (null until the environment installs them); fatal
// paths (Fog message + _exit(-1)) are modelled as returns, as in the other d2common batch files.
#include <cstddef>
#include <cstdint>

#include <emscripten.h>

#include "D2BitManip.h"
#include "D2Monsters.h"
#include "D2StatList.h"
#include "DataTbls/ItemsTbls.h"
#include "Drlg/D2DrlgDrlg.h"
#include "Path/Path.h"

// The vendored path/stat-list structs carry our engine's 8-byte pointers, so their offsets differ
// from retail's; the 1.13c offsets below are matched to fields by name (retail room coordinates
// 0x4C..0x58 are D2DrlgCoordsStrc's subtile box, path 0x28/0x34/0x9C the count, flags and points).
static_assert(offsetof(D2DynamicPathStrc, pRoom) == 0x1C, "path pRoom");
static_assert(offsetof(D2ItemStatCostTxt, nKeepZero) == 0x50, "itemstatcost keep zero");
static_assert(offsetof(D2ItemStatCostTxt, bHasOpApplyingToItem) == 0x53, "itemstatcost op-to-item flag");

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t Rd32(const void* b, int o)
	{
		uint32_t v;
		__builtin_memcpy(&v, static_cast<const uint8_t*>(b) + o, sizeof(v));
		return v;
	}
	inline void Wr32(void* b, int o, uint32_t v)
	{
		__builtin_memcpy(static_cast<uint8_t*>(b) + o, &v, sizeof(v));
	}
}

// Retail .data tables shared with the +0x37920/+0x37CA0 rows: six 3-dword records each, [0] scaled
// count, [1] value. Defined and filled by the module's loader (d2common_batch_37ca0.cpp).
extern "C"
{
	extern uint32_t g_d2common_dat_6fdeb160[18];
	extern uint32_t g_d2common_dat_6fdeb1f0[18];
}

// Env seams for callees that are still-UNMAPPED rows. The decompile shows no visible arguments for
// either call; the 1.13c register set-up at the call site is not recovered yet.
extern "C"
{
	int32_t (*g_d2common_fn_1330)(void) = nullptr;   // 0x1330 DUNGEON_GetRoomAtPosition
	int32_t (*g_d2common_fn_2D320)(void) = nullptr;  // 0x2D320 DRLG_IsTownLevel
}

// 1.13c D2Common+0x34E70: keep PATH flag bit 0 clear while every path point lies inside the path's
// current room. The flag is cleared first; with at least one point the room (path+0x1C) must exist
// and each point's X/Y (words at 0x9C/0x9E + i*4) must satisfy x in [room+0x4C, +0x4C+0x54) and
// y in [room+0x50, +0x50+0x58); the first point outside the box, or a missing room, sets bit 0.
// A null path is fatal in retail; we return.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_34e70(uint32_t pPath)
{
	D2DynamicPathStrc* path = reinterpret_cast<D2DynamicPathStrc*>(static_cast<uintptr_t>(pPath));
	if (!path)
		return;

	path->dwFlags &= ~1u;
	if (path->dwPathPoints <= 0)
		return;

	const D2ActiveRoomStrc* room = path->pRoom;
	int32_t i = 0;
	for (;;)
	{
		if (!room)
			break;

		const int32_t nX = static_cast<int32_t>(path->PathPoints[i].X);
		const int32_t nRoomX = room->tCoords.nSubtileX;
		if (nX < nRoomX || nRoomX + room->tCoords.nSubtileWidth <= nX)
			break;

		const int32_t nY = static_cast<int32_t>(path->PathPoints[i].Y);
		const int32_t nRoomY = room->tCoords.nSubtileY;
		if (nY < nRoomY || nRoomY + room->tCoords.nSubtileHeight <= nY)
			break;

		++i;
		if (path->dwPathPoints <= i)
			return;
	}
	path->dwFlags |= 1u;
}

// 1.13c D2Common+0x356E0: town-movement predicate. The second argument must point at a dword equal
// to 1; the unit must not be a town-eligible monster (0x1A660), and when this+0x0C is zero the room
// is looked up through 0x1330 (called twice, only the second result is kept, and a null answer
// returns 1) and 0x2D320 must answer zero. Every other path answers 1.
// TODO(spec): the decompile shows neither 0x1330's arguments nor 0x2D320's argument; both calls are
// made through seams with no visible arguments for now.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_356e0(uint32_t pThis, uint32_t pUnit, const int32_t* pParam)
{
	if (pParam && *pParam == 1 &&
	    !MONSTERS_CanBeInTown(reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pUnit))))
	{
		if (Rd32(P(pThis), 0x0C) == 0)
		{
			if (g_d2common_fn_1330)
				g_d2common_fn_1330();
			if (!g_d2common_fn_1330 || g_d2common_fn_1330() == 0)
				return 1;
		}
		if (g_d2common_fn_2D320 && g_d2common_fn_2D320() != 0)
			return 0;
	}
	return 1;
}

// 1.13c D2Common+0x37170: refresh a path/game record's value and scaled count from its act index.
// record+0x30 selects the table (0: 6fdeb160, else 6fdeb1f0); +0x04 takes entry index*3+1 and +0x08
// takes record+0x28 multiplied by entry index*3, where index is record+0x00.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_37170(uint32_t pRecord)
{
	uint8_t* rec = P(pRecord);
	if (!rec)
		return;

	const uint32_t nIndex = Rd32(rec, 0x00) * 3;
	const uint32_t* pTable =
	    Rd32(rec, 0x30) == 0 ? g_d2common_dat_6fdeb160 : g_d2common_dat_6fdeb1f0;
	Wr32(rec, 0x04, pTable[nIndex + 1]);
	Wr32(rec, 0x08, Rd32(rec, 0x28) * pTable[nIndex]);
}

// 1.13c D2Common+0x38940: store one value in a stat list's full stat array (the 1.13c split of
// STATLIST_SetUnitStatNewValue). The list is the hidden this (EBX): +0x48 is the stat array, +0x10
// the flags, +0x5C the value-change callback. The packed layer/stat id is looked up in the array
// (0x382B0); a missing entry is only inserted when the new value is non-zero (0x38640). A zero value
// for a row without nKeepZero removes the entry (0x381A0), otherwise the value is written and
// bHasOpApplyingToItem raises STATLIST_PERMANENT (0x20000000) on the list. When the value changed,
// the callback is set and the row carries the callback flag, the callback runs (unit, packed id,
// old, new): 1.13c passes four arguments where 1.10f passed six.
// TODO(spec): the decompile renders the callback-flag mask as byte 0xC of the external gdwBitMasks
// array, i.e. dwItemStatFlags bit 0x08 (ITEMSTATCOSTFLAG_FCALLBACK) tested in flags byte +5.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_38940(uint32_t pStatList, uint32_t nLayerStatId,
                                                int32_t nNewValue, uint32_t pItemStatCost,
                                                uint32_t pUnit)
{
	D2StatListExStrc* pStatListEx =
	    reinterpret_cast<D2StatListExStrc*>(static_cast<uintptr_t>(pStatList));
	const D2ItemStatCostTxt* pRow =
	    reinterpret_cast<const D2ItemStatCostTxt*>(static_cast<uintptr_t>(pItemStatCost));
	if (!pStatListEx || !pRow)
		return;

	D2StatsArrayStrc* pStatsArray = &pStatListEx->FullStats;
	const int32_t nIndex = STATLIST_FindStatIndex_6FDB6300(
	    pStatsArray, static_cast<D2SLayerStatIdStrc::PackedType>(nLayerStatId));
	D2StatStrc* pStat = nIndex >= 0 ? &pStatsArray->pStat[nIndex] : nullptr;

	if (!pStat)
	{
		if (nNewValue == 0)
			return;
		pStat = STATLIST_InsertStatOrFail_6FDB6970(
		    pStatListEx->pMemPool, pStatsArray,
		    static_cast<D2SLayerStatIdStrc::PackedType>(nLayerStatId));
		if (!pStat)
			return;
	}

	const int32_t nPreviousValue = pStat->nValue;
	if (nNewValue == 0 && pRow->nKeepZero == 0)
	{
		STATLIST_RemoveStat_6FDB6A30(pStatListEx->pMemPool, pStatsArray, pStat);
	}
	else
	{
		pStat->nValue = nNewValue;
		if (pRow->bHasOpApplyingToItem)
			pStatListEx->dwFlags |= STATLIST_PERMANENT;
	}

	const uint8_t nRowFlagByte = reinterpret_cast<const uint8_t*>(pRow)[5];
	if (nPreviousValue != nNewValue && pStatListEx->pfOnValueChanged &&
	    (static_cast<uint8_t>(gdwBitMasks[3]) & nRowFlagByte) != 0)
	{
		using Callback4 = void (*)(uint32_t, uint32_t, int32_t, int32_t);
		reinterpret_cast<Callback4>(pStatListEx->pfOnValueChanged)(
		    pUnit, nLayerStatId, nPreviousValue, nNewValue);
	}
}
