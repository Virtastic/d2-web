// 1.13c D2Common implementation batch (rows 0x2E990, 0x4A450, 0x2E960, 0x4DFD0, 0x30830, 0x390A0).
// Each body is written from the pinned 1.13c decompile and its disassembly; where the vendored
// D2MOO 1.10f twin is the same body the twin is called by name.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2StatList.h"
#include "Drlg/D2DrlgDrlg.h"
#include "Units/Units.h"

namespace
{
	// Retail 0x6FDE9EC8: 23 profiling records of 0x78 bytes. The reset zeroes the first three
	// dwords (low/high clock delta, count) of each record.
	struct PerfSlot
	{
		uint32_t nLow;
		int32_t nHigh;
		uint32_t nCount;
		uint32_t nRest[(0x78 - 3 * 4) / 4];
	};
	static_assert(sizeof(PerfSlot) == 0x78, "profiling slot stride");
	static_assert(sizeof(PerfSlot) * 23 == 0xAC8, "23 profiling slots");
	PerfSlot gPerfResetSlots[23];
}

// 1.13c D2Common+0x2E990 (#10169): stores arg2 at +0x7C of arg1, the D2ActiveRoomStrc::pRoomNext
// link (field identified by offset; the only caller is the export thunk). Null arg1 takes the
// module's fatal path (0x150B), which never returns, so nothing is stored. Retail stores a
// four-byte image; our rooms are native pointers.
EMSCRIPTEN_KEEPALIVE void D2Common_2E990_SetRoomNext(D2ActiveRoomStrc* pRoom, D2ActiveRoomStrc* pNext)
{
	if (!pRoom)
	{
		return;
	}
	pRoom->pRoomNext = pNext;
}

// 1.13c D2Common+0x4A450 (#10173): profiling reset. The retail body first calls 0x4A3D0, which
// logs and dumps the table; that function is still UNMAPPED, so the dump is the one seam left.
// It then zeroes the clock delta and count of all 23 records starting at 0x6FDE9EC8.
EMSCRIPTEN_KEEPALIVE void D2Common_4A450_ProfileReset()
{
	for (PerfSlot& slot : gPerfResetSlots)
	{
		slot.nLow = 0;
		slot.nHigh = 0;
		slot.nCount = 0;
	}
}

// 1.13c D2Common+0x2E960 (#10181): returns the +0x7C dword of arg1, the D2ActiveRoomStrc::pRoomNext
// link (field identified by offset; the only caller is the export thunk). Null arg1 takes the
// module's fatal path (0x1513), which never returns, so the read is never reached.
EMSCRIPTEN_KEEPALIVE D2ActiveRoomStrc* D2Common_2E960_GetRoomNext(D2ActiveRoomStrc* pRoom)
{
	if (!pRoom)
	{
		return nullptr;
	}
	return pRoom->pRoomNext;
}

// 1.13c D2Common+0x4DFD0 (#10179): returns the +0x20 dword of arg1, the
// D2ActiveRoomStrc::pRoomTiles pointer (field identified by offset; the only caller is the export
// thunk). Null arg1 takes the module's fatal path (0xA65), which never returns.
EMSCRIPTEN_KEEPALIVE D2DrlgRoomTilesStrc* D2Common_4DFD0_GetRoomTiles(D2ActiveRoomStrc* pRoom)
{
	if (!pRoom)
	{
		return nullptr;
	}
	return pRoom->pRoomTiles;
}

// 1.13c D2Common+0x30830 (#10172): overlay height of a unit. Null unit reads as type 6, so it
// falls through to 0. Type 0 (player) is 1; type 1 (monster) is MonStats2.nOverlayHeight (0x0C)
// minus one, or 0 when the row is missing; any other type is 0. Same body as the vendored
// D2MOO 1.10f UNITS_GetOverlayHeight.
EMSCRIPTEN_KEEPALIVE int D2Common_30830_UNITS_GetOverlayHeight(D2UnitStrc* pUnit)
{
	return UNITS_GetOverlayHeight(pUnit);
}

// 1.13c D2Common+0x390A0 (#10186): the unit's total maximum-mana stat. Guards: null unit, null
// stat list, fewer than 10 ItemStatCost rows (the row at index 9 is STAT_MAXMANA), null table.
// The call is STATLIST_GetTotalStat with packed id 0x90000 (stat 9, layer 0).
EMSCRIPTEN_KEEPALIVE int D2Common_390A0_GetUnitMaxManaTotal(D2UnitStrc* pUnit)
{
	if (!pUnit || !pUnit->pStatListEx)
	{
		return 0;
	}
	if (sgptDataTables->nItemStatCostTxtRecordCount <= 9 || !sgptDataTables->pItemStatCostTxt)
	{
		return 0;
	}
	return STATLIST_GetTotalStat_6FDB63E0(
		STATLIST_StatListExCast(pUnit->pStatListEx),
		D2SLayerStatIdStrc::MakeFromStatId(STAT_MAXMANA).nPackedValue,
		&sgptDataTables->pItemStatCostTxt[9]);
}
