// 1.13c D2Common+0x2CC60 (sub_2CC60, ordinal 11097): stdcall(pUnit), returns the hireling revive cost.
// Retail: level is 0 when pUnit is null, its extended stat list (+0x5c) is null, or the ItemStatCost count (+0xBD4) is 12 or less.
// Otherwise level is the total of stat 12 (Level, layer 0) read from that list by the 1.13c 0x38A80 (the vendored
// STATLIST_GetTotalStat_6FDB63E0), with ItemStatCost row 12 as the record. cost = ((level*level) / 2) * 15 in 32-bit
// wrap, signed halving, then clamped to 50000 unsigned.
#include <cstdint>
#include <emscripten.h>
#include "D2DataTbls.h"
#include "D2StatList.h"
#include "Units/Units.h"

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Common_2CC60_HirelingReviveCost(D2UnitStrc* unit)
{
	int32_t level = 0;
	if (unit && unit->pStatListEx && sgptDataTables->nItemStatCostTxtRecordCount > STAT_LEVEL && sgptDataTables->pItemStatCostTxt)
	{
		level = STATLIST_GetTotalStat_6FDB63E0(unit->pStatListEx, D2SLayerStatIdStrc::Make(0, STAT_LEVEL).nPackedValue, &sgptDataTables->pItemStatCostTxt[STAT_LEVEL]);
	}
	const uint32_t square = static_cast<uint32_t>(level) * static_cast<uint32_t>(level);
	const uint32_t cost = static_cast<uint32_t>(static_cast<int32_t>(square) / 2) * 15u;
	return cost > 50000u ? 50000u : cost;
}
