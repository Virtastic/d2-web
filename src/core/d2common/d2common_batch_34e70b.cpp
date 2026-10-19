// 1.13c D2Common implementation batch (rows 0x7F490, 0x6AB20, 0x6B180, 0x1E420).
// Bodies follow the cached 1.13c decompiles (build-wasm/ghidra-d2common/decomp/0x<RVA>.c).
// Every one of these takes operands retail passes in registers the decompiler does not name
// (0x7F490: x in EDX, y in EAX, radius in EDI; 0x6AB20: unit in EAX, percent in ESI;
// 0x6B180: unit in EAX; 0x1E420: class-id-holder in EAX); they are declared here as ordinary
// parameters in the order the body reads them. Retail's fatal asserts (_exit after a Fog message)
// are modelled as the caller's neutral result.
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Dungeon.h"
#include "D2Skills.h"
#include "D2StatList.h"
#include "DataTbls/ItemsTbls.h"
#include "DataTbls/MonsterTbls.h"
#include "Drlg/D2DrlgDrlg.h"
#include "Units/Units.h"
#include "core/prng.h"

static_assert(offsetof(D2ItemsTxt, nArmorComp) == 0x116, "items.txt nArmorComp");

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline int32_t I32(const uint8_t* p, int32_t o) { return *reinterpret_cast<const int32_t*>(p + o); }

	inline int32_t StatTotal(uint8_t* pUnit, int32_t nStatId)
	{
		if (!pUnit || !I32(pUnit, 0x5C) || !sgptDataTables || !sgptDataTables->pItemStatCostTxt ||
			nStatId < 0 || nStatId >= sgptDataTables->nItemStatCostTxtRecordCount)
			return 0;
		return STATLIST_GetTotalStat_6FDB63E0(
			reinterpret_cast<D2StatListStrc*>(P(static_cast<uint32_t>(I32(pUnit, 0x5C)))),
			D2SLayerStatIdStrc::MakeFromStatId(static_cast<uint16_t>(nStatId)).nPackedValue,
			&sgptDataTables->pItemStatCostTxt[nStatId]);
	}
}

// 1.13c D2Common+0x7F490: per-room guard of #10406/#10407. Takes the room, the search point and
// the search radius (retail: ECX=room, EDX=x, EAX=y, EDI=radius), fetches the room's subtile
// coordinates and answers whether the radius square overlaps the room box. The x test can only
// reject when both ends fail (as in the 1.10f inline || chain); the y test is the same ||.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_7f490(uint32_t pRoom, int32_t nX,
	int32_t nY, int32_t nRadius)
{
	if (!pRoom)
		return 0;
	D2DrlgCoordsStrc coords = {};
	DUNGEON_GetRoomCoordinates(reinterpret_cast<D2ActiveRoomStrc*>(P(pRoom)), &coords);

	if (nX + nRadius < coords.nSubtileX)
	{
		if (coords.nSubtileWidth + coords.nSubtileX < nX - nRadius)
			return 0;
		if (coords.nSubtileY <= nY + nRadius)
			return 1;
	}
	else if (coords.nSubtileY <= nY + nRadius)
	{
		return 1;
	}

	return nY - nRadius <= coords.nSubtileHeight + coords.nSubtileY;
}

// 1.13c D2Common+0x6AB20: damage-percent gate for missile damage. Category 1/2/5 asks the owner's
// stat list for stat 0x149/0x14A/0x14C; category 4/12 asks for itemstatcost row 0x14B (331) and
// additionally requires a unit stat list. A zero stat returns 0, otherwise the retail ratio
// (100, percent, 100) is returned unchanged. Unknown categories return 0.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_6ab20(uint32_t pUnit, int32_t nCategory,
	int32_t nPercent)
{
	uint8_t* u = P(pUnit);
	int32_t nStatId = -1;

	switch (nCategory)
	{
	case 1:
		nStatId = 0x149;
		break;
	case 2:
		nStatId = 0x14A;
		break;
	case 5:
		nStatId = 0x14C;
		break;
	case 4:
	case 0xC:
		if (!u || !I32(u, 0x5C))
			return 0;
		if (!sgptDataTables || !sgptDataTables->pItemStatCostTxt ||
			sgptDataTables->nItemStatCostTxtRecordCount < 0x14C)
			return 0;
		if (StatTotal(u, 0x14B) == 0)
			return 0;
		return DATATBLS_ApplyRatio(100, nPercent, 100);
	default:
		return 0;
	}

	if (!u)
		return 0;
	// The 0x38B70 callee is a two-argument (unit, statId) wrapper of the stat-list lookup; the
	// 1.10f equivalent is STATLIST_UnitGetStatValue with layer 0.
	if (STATLIST_UnitGetStatValue(reinterpret_cast<const D2UnitStrc*>(u), nStatId, 0) == 0)
		return 0;
	return DATATBLS_ApplyRatio(100, nPercent, 100);
}

// 1.13c D2Common+0x6B180: three-pass critical-strike roll against the owner (retail EAX). Pass 1
// tests itemstatcost row 0x151 (337); pass 2 requires a stat list and row count above 0x8D and
// tests row 0x8D (141); pass 3 adds the weapon-mastery bonus of type 2 for the item argument. Each
// pass advances the unit's 8-byte seed at +0x20 (one 64-bit LCG step) and compares low % 100
// (signed modulo) against the pass's percentage. A pass with percentage <= 0 never fires but the
// seed still advances first for pass 1.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_6b180(uint32_t pUnit, uint32_t pItem)
{
	uint8_t* u = P(pUnit);
	uint8_t* item = P(pItem);
	if (!u)
		return 0;

	int32_t nChance = StatTotal(u, 0x151);
	d2::Seed& seed = *reinterpret_cast<d2::Seed*>(u + 0x20);

	if (static_cast<int32_t>(static_cast<uint32_t>(d2::roll(seed))) % 100 < nChance)
		return 1;

	if (I32(u, 0x5C) && sgptDataTables && sgptDataTables->pItemStatCostTxt &&
		sgptDataTables->nItemStatCostTxtRecordCount > 0x8D)
	{
		nChance = StatTotal(u, 0x8D);
		if (nChance != 0 &&
			static_cast<int32_t>(static_cast<uint32_t>(d2::roll(seed))) % 100 < nChance)
			return 1;
	}

	if (item)
	{
		nChance = SKILLS_GetWeaponMasteryBonus(reinterpret_cast<D2UnitStrc*>(u),
			reinterpret_cast<D2UnitStrc*>(item), nullptr, 2);
		if (nChance != 0 &&
			static_cast<int32_t>(static_cast<uint32_t>(d2::roll(seed))) % 100 < nChance)
			return 1;
	}

	return 0;
}

// 1.13c D2Common+0x1E420: copy the six armor components (items.txt +0x116, nArmorComp) of the
// item class held at source+4 (a unit's dwClassId / an inventory node's class field) into the
// caller's 6-byte context. A null source, a null table or an out-of-range class zeroes the six
// bytes; retail reports a fatal error for the latter two and exits, which cannot be replayed.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_1e420(uint32_t pOut, uint32_t pSource)
{
	uint8_t* out = P(pOut);
	if (!out)
		return;

	if (!pSource)
	{
		std::memset(out, 0, 6);
		return;
	}

	const D2ItemsTxt* pRow = DATATBLS_GetItemsTxtRecord(I32(P(pSource), 4));
	if (!pRow)
	{
		std::memset(out, 0, 6);
		return;
	}

	std::memcpy(out, pRow->nArmorComp, 6);
}
