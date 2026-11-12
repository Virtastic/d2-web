// 1.13c D2Common implementation batch: +0x22590, +0x2FED0, +0x39050, +0x38E80, +0x38C20, +0x2D070.
// Bodies translated from the pinned 1.13c decompiles. Retail pointers are 32-bit image addresses in
// the offsets below; unit fields with a vendored twin are read by name. Callees that are still
// UNMAPPED rows are env-seam function pointers (null until the environment installs them). Fatal
// paths (Fog message + _exit(-1)) are modelled as returns, as in the other d2common batch files.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2StatList.h"
#include "DataTbls/ItemsTbls.h"
#include "Units/Item.h"
#include "Units/Units.h"
#include "Path/Path.h"

static_assert(offsetof(D2ItemsTxt, dwBitField1) == 0xDC, "items.txt bitfield 1");
static_assert(offsetof(D2ItemsTxt, nInvWidth) == 0x10F, "items.txt inventory width");
static_assert(offsetof(D2ItemsTxt, nInvHeight) == 0x110, "items.txt inventory height");

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& W(void* b, int o) { return *reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(b) + o); }
}

// Env seams for callees that are still-UNMAPPED rows. Signatures follow the 1.13c calls; a null
// pointer means the environment has not installed the implementation yet.
extern "C"
{
	// 0x38B00: stat value from a stat list (EBX = list, EDI = packed layer/stat id).
	int32_t (*g_d2common_fn_38B00)(uint32_t, int32_t) = nullptr;
}

// 1.13c D2Common+0x22590 (#10572): the two dwords at pItemData+0x1C/+0x20 for a UNIT_ITEM with a
// non-null item data block; everything else (null unit, another unit type, null data) writes 0/0.
extern "C"
EMSCRIPTEN_KEEPALIVE void ITEMS_GetRealmData(uint32_t pUnit, uint32_t* pOut1C, uint32_t* pOut20)
{
	D2UnitStrc* unit = reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pUnit));
	if (unit && unit->dwUnitType == UNIT_ITEM)
	{
		D2ItemDataStrc* pItemData = unit->pItemData;
		if (pItemData)
		{
			*pOut1C = pItemData->dwRealmData[0];
			*pOut20 = pItemData->dwRealmData[1];
			return;
		}
	}
	*pOut1C = 0;
	*pOut20 = 0;
}

// 1.13c D2Common+0x2FED0 (#10574, D2SetEnabledStat): sets the unit's static path (unit+0x2C) to a
// room and a game position: +0x00 room, +0x0C/+0x10 game X/Y, +0x04 client X = (X - Y) * 0x10,
// +0x08 client Y = (X + Y) * 8, +0x1C direction byte = 0. Written by field: the wasm64 path holds an
// 8-byte room pointer, so the retail dword offsets do not address it. A null unit is fatal 0x866 in
// retail; we return.
extern "C"
EMSCRIPTEN_KEEPALIVE void D2SetEnabledStat(D2UnitStrc* unit, D2ActiveRoomStrc* pRoom, int32_t nX,
                                                int32_t nY)
{
	if (!unit)
		return;
	D2StaticPathStrc* path = unit->pStaticPath;
	path->pRoom = pRoom;
	path->tGameCoords.nX = nX;
	path->dwClientCoordX = (nX - nY) * 0x10;
	path->nDirection = 0;
	path->tGameCoords.nY = nY;
	path->dwClientCoordY = (nX + nY) * 8;
}

// 1.13c D2Common+0x39050 (#10578): the unit's total stat 11 (max stamina, layer 0, packed
// 0xB0000) from ItemStatCost row 11. Guards: null unit, null stat list, fewer than 12 rows.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t UNITS_GetTotalMaxStamina_39050(D2UnitStrc* pUnit)
{
	if (!pUnit || !pUnit->pStatListEx)
		return 0;
	if (!sgptDataTables || !sgptDataTables->pItemStatCostTxt)
		return 0;
	if (sgptDataTables->nItemStatCostTxtRecordCount <= 0xB)
		return 0;
	return STATLIST_GetTotalStat_6FDB63E0(pUnit->pStatListEx,   // the list itself: 0x38A80 picks the full-stats array by the list's own flag
		D2SLayerStatIdStrc::MakeFromStatId(0xB).nPackedValue,
		&sgptDataTables->pItemStatCostTxt[0xB]);
}

// 1.13c D2Common+0x38C20 (#10587): stat value (layer nLayer, stat nStatId) of a unit, from
// ItemStatCost row nStatId through the 0x38B00 getter. Guards: null unit, nStatId out of range,
// null row, null stat list.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_port_d2common_38c20(D2UnitStrc* pUnit, int32_t nStatId,
                                                  int32_t nLayer)
{
	if (!pUnit)
		return 0;
	if (!sgptDataTables || !sgptDataTables->pItemStatCostTxt)
		return 0;
	if (nStatId < 0 || nStatId >= sgptDataTables->nItemStatCostTxtRecordCount)
		return 0;
	if (!pUnit->pStatListEx)
		return 0;
	const int32_t nPacked = (nStatId << 16) | (nLayer & 0xFFFF);
	return STATLIST_GetBaseStat_6FDB6340(STATLIST_StatListExCast(pUnit->pStatListEx),
		D2SLayerStatIdStrc::Make(static_cast<uint16_t>(nLayer), nStatId).nPackedValue,
		&sgptDataTables->pItemStatCostTxt[nStatId]);
}

