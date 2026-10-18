// 1.13c D2Common implementation batch: +0x4AD20, +0x24800, +0x30340, +0x31200, +0x54A00,
// +0x45BE0, +0x2CC60, +0x4FB30, +0x26930, +0x303E0, +0x21DD0, +0x38710. Bodies follow the
// cached 1.13c decompile; where a vendored D2MOO function is the same body it is called rather
// than copied. Retail's fatal paths (Fog message + _exit(-1)) are modelled as returns.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

#include "D2Collision.h"
#include "D2Inventory.h"
#include "D2Items.h"
#include "D2Skills.h"
#include "D2StatList.h"
#include "DataTbls/MonsterTbls.h"
#include "Units/Units.h"

// Defined by the +0x230A0 row (d2common_batch_1a330.cpp).
int32_t D2Common_230A0_ItemTypeFromItemId(uint32_t nItemId);

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t R(const uint8_t* b, int o) { return *reinterpret_cast<const uint32_t*>(b + o); }
}

// +0x4AD20: Bresenham-style walk over room collision cells from the begin coord to the end coord.
// Returns 1 as soon as a cell matches the mask (or the walk leaves the room, or the room is null,
// in which case the end coord is set to the begin coord), 0 when the path stays clear. The vendored
// COLLISION_RayTrace is this body.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_4ad20(uint32_t pRoom, uint32_t pBeginCoord, uint32_t pEndCoord, uint32_t nCollisionMask) {   // 1.13c D2Common+0x4AD20
	return COLLISION_RayTrace(reinterpret_cast<D2ActiveRoomStrc*>(pRoom), reinterpret_cast<D2CoordStrc*>(pBeginCoord), reinterpret_cast<D2CoordStrc*>(pEndCoord), static_cast<uint16_t>(nCollisionMask)) ? 1u : 0u;
}

// +0x24800: dword at +0xEC of the items.txt record named by the item's class id (+0x04). A null
// unit, a non-item unit type (not 4) or a missing record is retail's fatal path; we answer 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_24800(uint32_t pItem) {   // 1.13c D2Common+0x24800
	if (!pItem || R(P(pItem), 0x00) != 4)
		return 0;
	return static_cast<uint32_t>(ITEMS_GetSpawnStack(reinterpret_cast<D2UnitStrc*>(pItem)));
}

// +0x30340: precision X. Unit types 2 (object), 4 (item) and 5 (tile) take the static-path game
// coord x at +0x0C shifted left 16; types 0, 1 and 3 take the dynamic-path fixed-point x at +0.
// A null unit is retail's fatal path; we answer 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_30340(uint32_t pUnit) {   // 1.13c D2Common+0x30340
	if (!pUnit)
		return 0;
	return static_cast<uint32_t>(UNITS_GetPrecisionX(reinterpret_cast<D2UnitStrc*>(pUnit)));
}

// +0x31200: a monster can switch AI when its id is in monsters.txt, its MonStatsEx (+0x18) names a
// monstats2 record, that record's mode flags carry walking, and the monster's own flags (+0x0C)
// have boss clear and switch-AI set. Same body as the vendored UNITS_CanSwitchAI.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_31200(uint32_t nMonsterId) {   // 1.13c D2Common+0x31200
	return UNITS_CanSwitchAI(static_cast<int>(nMonsterId)) ? 1u : 0u;
}

// +0x54A00: per-level monster stat block. The difficulty is clamped to 0..2 and the level to the
// monlvl record count - 1; the monlvl row is then read with the game-type offset of 3 and, for each
// set flag bit, HP/AC/EXP/TH/A1/A2/S1/elemental min and max are either scaled by the monlvl ratio
// or copied raw when the monster's NORATIO flag is set. Writes the 14-int D2MonStatsInitStrc.
// The vendored DATATBLS_CalculateMonsterStatsByLevel has this exact structure and output layout.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_54a00(uint32_t nMonsterId, uint32_t nGameType, uint32_t nDifficulty, uint32_t nLevel, uint32_t nFlags, uint32_t pOutStats) {   // 1.13c D2Common+0x54A00
	if (!pOutStats)
		return 0;
	return DATATBLS_CalculateMonsterStatsByLevel(static_cast<int>(nMonsterId), static_cast<int>(nGameType), static_cast<int>(nDifficulty), static_cast<int>(nLevel), static_cast<short>(nFlags), reinterpret_cast<D2MonStatsInitStrc*>(pOutStats)) ? 1u : 0u;
}

// +0x45BE0: affix applicability test. The affix byte block has two type-class gates (bytes 1 and 0
// with item type selectors 0x2D and 0x32) that accept anything whose items.txt type is outside the
// lists {0x18,0x19,0x1A,0x1B,0x23} and {2,0x0F,0x10,0x13}; otherwise the item type must match one
// of the nine byte-indexed ids in the function's table with a non-zero flag byte. A null unit is
// retail's fatal path; we answer 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_45be0(uint32_t pUnit, uint32_t pAffixData) {   // 1.13c D2Common+0x45BE0
	if (!pUnit)
		return 0;
	const uint8_t* pAffix = P(pAffixData);
	int32_t nItemType = 0;
	if (R(P(pUnit), 0x00) == 4)
		nItemType = D2Common_230A0_ItemTypeFromItemId(R(P(pUnit), 0x04));

	if (pAffix[1] != 0
		&& ITEMS_CheckItemTypeId(reinterpret_cast<const D2UnitStrc*>(pUnit), 0x2D)
		&& nItemType != 0x1A && nItemType != 0x1B && nItemType != 0x23
		&& nItemType != 0x18 && nItemType != 0x19)
	{
		return 1;
	}

	if (pAffix[0] != 0
		&& ITEMS_CheckItemTypeId(reinterpret_cast<const D2UnitStrc*>(pUnit), 0x32)
		&& nItemType != 2 && nItemType != 0x0F && nItemType != 0x10 && nItemType != 0x13)
	{
		return 1;
	}

	static const struct
	{
		uint8_t nFlagByte;
		int32_t nItemType;
	} aTable[9] = {
		{ 2, 2 }, { 3, 0x18 }, { 4, 0x19 }, { 5, 0x1A }, { 6, 0x1B },
		{ 6, 0x23 }, { 7, 0x0F }, { 8, 0x10 }, { 9, 0x13 },
	};

	for (int nCounter = 0; nCounter < 9; ++nCounter)
	{
		if (nItemType == aTable[nCounter].nItemType && pAffix[aTable[nCounter].nFlagByte] != 0)
			return 1;
	}
	return 0;
}
