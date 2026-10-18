// 1.13c D2Common implementation batch: +0x38DC0, +0x2A3D0, +0x22A70, +0x35B00, +0x4F390, +0x2CA10.
// Bodies translated from the pinned 1.13c decompiles. Unit fields with a vendored twin are read by
// name; opaque pointed-to blocks keep their 1.13c offsets. Callees that are still UNMAPPED rows are
// env-seam function pointers (null until the environment installs them). Fatal paths (Fog message +
// _exit(-1)) are modelled as returns, as in the other d2common batch files.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Skills.h"
#include "DataTbls/ItemsTbls.h"
#include "Path/Path.h"
#include "Units/Item.h"
#include "Units/Units.h"
#include "core/prng.h"

static_assert(offsetof(D2ItemDataStrc, dwItemFlags) == 0x18, "item data flags");
static_assert(offsetof(D2ItemsTxt, dwBitField1) == 0xDC, "items.txt bitfield 1");
static_assert(offsetof(D2ItemsTxt, nInvWidth) == 0x10F, "items.txt inventory width");
static_assert(offsetof(D2ItemsTxt, nInvHeight) == 0x110, "items.txt inventory height");

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline int32_t& I32(void* b, int o) { return *reinterpret_cast<int32_t*>(static_cast<uint8_t*>(b) + o); }
	inline int32_t RdI32(const void* b, int o)
	{
		int32_t v;
		std::memcpy(&v, static_cast<const uint8_t*>(b) + o, sizeof(v));
		return v;
	}
}

// Env seams for callees that are still-UNMAPPED rows. Signatures follow the 1.13c calls; a null
// pointer means the environment has not installed the implementation yet.
extern "C"
{
	// 0x38B70: unit's total value of ItemStatCost row nStatId.
	int32_t (*g_d2common_fn_38B70)(uint32_t, int32_t) = nullptr;
	// 0x24610: item's socket/affix count (low byte of the result); the seam is defined in
	// d2common_batch_37ca0.cpp with a void* argument.
	extern uint32_t (*g_d2common_fn_24610)(void*);
	// 0x3A740: apply stat/value pair to a unit's stat list.
	void (*g_d2common_fn_3A740)(uint32_t, uint32_t, int32_t) = nullptr;
}

// 1.13c D2Common+0x38DC0 (#10597): forwards to the stat getter 0x38B70 with its own second and
// third arguments; the first argument is unused.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_38dc0(uint32_t nUnused, uint32_t pUnit, int32_t nStatId)
{
	(void)nUnused;
	if (!g_d2common_fn_38B70)
		return;
	g_d2common_fn_38B70(pUnit, nStatId);
}

// 1.13c D2Common+0x2A3D0 (#10599): for a UNIT_ITEM, set the number of sockets added by the item's
// quality. The socket count is min(invWidth * invHeight, 6, 0x24610's low byte, nRequest), but the
// whole block is skipped when the item's inventory footprint is zero. The item's dwItemFlags gets
// bit 0x800 and stat 0xC2 is set to the result through 0x3A740.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_2a3d0(uint32_t pUnit, int32_t nRequest)
{
	D2UnitStrc* unit = reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pUnit));
	if (!unit || unit->dwUnitType != UNIT_ITEM)
		return;

	uint8_t nWidth = 0;
	uint32_t nHeight = 0;
	const D2ItemsTxt* pRow = DATATBLS_GetItemsTxtRecord(unit->dwClassId);
	if (pRow)
	{
		nWidth = pRow->nInvWidth;
		nHeight = pRow->nInvHeight;
	}
	const uint32_t nCells = nHeight * nWidth;
	if (nCells == 0)
		return;

	uint32_t nSockets = nCells < 6 ? nCells : 6;
	if (!g_d2common_fn_24610)
		return;
	const uint32_t nRolled =
	    g_d2common_fn_24610(reinterpret_cast<void*>(static_cast<uintptr_t>(pUnit)));
	if ((nRolled & 0xFF) <= nSockets)
		nSockets = nRolled & 0xFF;

	if (nRequest < 2)
		nRequest = 1;
	if (static_cast<int32_t>(nSockets) <= nRequest)
		nRequest = static_cast<int32_t>(nSockets);

	if (unit->dwUnitType == UNIT_ITEM && unit->pItemData)
		unit->pItemData->dwItemFlags |= 0x800;

	if (g_d2common_fn_3A740)
		g_d2common_fn_3A740(pUnit, 0xC2, nRequest);
}

// 1.13c D2Common+0x22A70 (#10602): bit 3 of items.txt dwBitField1 for a UNIT_ITEM's class. A null
// unit (fatal 0xD9E), another unit type (0xD9F) or a missing row (0xDA1) is fatal in retail; we
// return 0 for all three.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_22a70(uint32_t pUnit)
{
	D2UnitStrc* unit = reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pUnit));
	if (!unit || unit->dwUnitType != UNIT_ITEM)
		return 0;
	const D2ItemsTxt* pRow = DATATBLS_GetItemsTxtRecord(unit->dwClassId);
	if (!pRow)
		return 0;
	return pRow->dwBitField1 & 8;
}

// 1.13c D2Common+0x35B00 (#10604, D2GetAffixBIN): two dwords derived from the unit's path velocity
// when the unit is in moving mode and its path has points. out1 = (vx - vy) >> 1 and
// out2 = (vy + vx) >> 2 after both velocities are shifted right by 11. Everything else writes 0/0.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_35b00(uint32_t pUnit, int32_t* pOutA, int32_t* pOutB)
{
	D2UnitStrc* unit = reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pUnit));
	if (!unit || !UNITS_IsInMovingMode(unit))
	{
		*pOutA = 0;
		*pOutB = 0;
		return;
	}
	D2DynamicPathStrc* pPath = unit->pDynamicPath;
	if (!pPath || pPath->dwPathPoints == 0)
	{
		*pOutA = 0;
		*pOutB = 0;
		return;
	}
	const int32_t nVX = pPath->tVelocityVector.nX >> 11;
	const int32_t nVY = pPath->tVelocityVector.nY >> 11;
	*pOutA = (nVX - nVY) >> 1;
	*pOutB = (nVY + nVX) >> 2;
}

// 1.13c D2Common+0x4F390 (#10586): reseed a unit from the skill node whose skills.txt id matches
// nSkillId and whose owner GUID is -1 (native skill). With nMode 0 the seed value is the path's
// target Y * 3 plus target X; otherwise it is (unit type << 16) + unit id. The node's nPar1/nPar2
// (at +0x18/+0x1C) are kept in step and the unit's seed becomes (value, 0x29A). A missing node is
// fatal 0xA53 in retail; we return.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_4f390(uint32_t pUnit, int32_t nMode, int16_t nSkillId)
{
	D2UnitStrc* unit = reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pUnit));
	if (!unit)
		return;

	D2SkillStrc* pFound = nullptr;
	if (unit->pSkills)
	{
		for (D2SkillStrc* pSkill = unit->pSkills->pFirstSkill; pSkill; pSkill = pSkill->pNextSkill)
		{
			if (pSkill->pSkillsTxt && pSkill->pSkillsTxt->nSkillId == nSkillId &&
			    pSkill->nOwnerGUID == 0xFFFFFFFF)
			{
				pFound = pSkill;
				break;
			}
		}
	}
	if (!pFound)
		return;

	int32_t nValue;
	if (nMode == 0)
	{
		if (!unit->pDynamicPath)
			return;
		nValue = unit->pDynamicPath->tTargetCoord.Y * 3 + unit->pDynamicPath->tTargetCoord.X;
	}
	else
	{
		nValue = static_cast<int32_t>((unit->dwUnitType << 16) + unit->dwUnitId);
	}

	int32_t& nPar1 = *reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(pFound) + 0x18);
	int32_t& nPar2 = *reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(pFound) + 0x1C);
	if (nPar1 == nValue)
	{
		const int32_t nOld = nPar2;
		nPar2 = nOld + 7;
		nPar1 = nValue;
		unit->pSeed.nLowSeed = static_cast<uint32_t>(nOld + 7);
		unit->pSeed.nHighSeed = 0x29A;
		return;
	}
	nPar2 = nValue;
	nPar1 = nValue;
	unit->pSeed.nLowSeed = static_cast<uint32_t>(nValue);
	unit->pSeed.nHighSeed = 0x29A;
}

// Retail .data tables read by 0x2CA10, indexed by quality class (0..3). Each base value is added to
// a bounded random roll and the sum scaled by the record stride. They are filled by the module's
// item-table loader; zero is the only value available until those rows are mapped.
extern "C"
{
	uint32_t g_d2common_dat_6fded014[4] = {};
	uint32_t g_d2common_dat_6fded024[4] = {};
	uint32_t g_d2common_dat_6fded038[4] = {};
	uint32_t g_d2common_dat_6fded048[4] = {};
	uint32_t g_d2common_dat_6fded05c[4] = {};
	uint32_t g_d2common_dat_6fded06c[4] = {};
	uint32_t g_d2common_dat_6fded080[4] = {};
	uint32_t g_d2common_dat_6fded090[4] = {};
	uint32_t g_d2common_dat_6fded0a0[8] = {};
	uint8_t g_d2common_dat_6fded0c0[0x60] = {};
	uint8_t g_d2common_dat_6fded120[0x60] = {};
	uint8_t g_d2common_dat_6fded180[0x90] = {};
	uint8_t g_d2common_dat_6fded210[0xF0] = {};
	uint8_t g_d2common_dat_6fded300[0x50] = {};
}

// 1.13c D2Common+0x2CA10 (#10606): fill an affix record from the quality class at pOut[0] (0..3)
// and the item seed {nSeedLow, 0x29A}. Each of four tables (prefix/suffix/thunder/small) is picked
// by a fresh SEED_RollLimitedRandomNumber bound from the matching count array and copied into the
// record together with its stat offsets. nClass != 0 applies the class bonus block at +0x1C stride.
// A null unit/record or a class outside 0..2 is a 0 return.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_2ca10(uint32_t nSeedLow, int32_t* pOut, uint8_t nClass)
{
	if (!nSeedLow || !pOut || nClass >= 3)
		return 0;

	const int32_t nIndex = pOut[0];
	if (nIndex < 0 || nIndex > 3)
		return 0;

	d2::Seed seed;
	seed.low = nSeedLow;
	seed.high = 0x29A;

	*reinterpret_cast<uint16_t*>(reinterpret_cast<uint8_t*>(pOut) + 0x38) = 0;
	pOut[0xD] = static_cast<int32_t>(g_d2common_dat_6fded0a0[nIndex * 2]);
	pOut[8] = static_cast<int32_t>(g_d2common_dat_6fded0a0[nIndex * 2 + 1]);

	// Table A: stride 0x10 (items 1 and 6, stat offsets +0x0C/+0x08).
	uint32_t nRoll = d2::rollLimited(seed, static_cast<int32_t>(g_d2common_dat_6fded024[nIndex]));
	int32_t nOffset = static_cast<int32_t>(nRoll + g_d2common_dat_6fded014[nIndex]) * 0x10;
	pOut[1] = RdI32(g_d2common_dat_6fded0c0, nOffset);
	pOut[6] = RdI32(g_d2common_dat_6fded0c0, nOffset + 4);
	pOut[8] += RdI32(g_d2common_dat_6fded0c0, nOffset + 0xC);
	pOut[0xD] += RdI32(g_d2common_dat_6fded0c0, nOffset + 0x8);

	// Table B: stride 0x10 (items 2 and 7, stat offsets +0x0C/+0x08).
	nRoll = d2::rollLimited(seed, static_cast<int32_t>(g_d2common_dat_6fded048[nIndex]));
	nOffset = static_cast<int32_t>(nRoll + g_d2common_dat_6fded038[nIndex]) * 0x10;
	pOut[2] = RdI32(g_d2common_dat_6fded120, nOffset);
	pOut[7] = RdI32(g_d2common_dat_6fded120, nOffset + 4);
	pOut[8] += RdI32(g_d2common_dat_6fded120, nOffset + 0xC);
	pOut[0xD] += RdI32(g_d2common_dat_6fded120, nOffset + 0x8);

	// Table C: stride 0x18 (items 3 and 5 split by class, items 9 and 10, flag word at +0x04).
	nRoll = d2::rollLimited(seed, static_cast<int32_t>(g_d2common_dat_6fded06c[nIndex]));
	nOffset = static_cast<int32_t>(nRoll + g_d2common_dat_6fded05c[nIndex]) * 0x18;
	if (nIndex < 3)
	{
		pOut[3] = RdI32(g_d2common_dat_6fded180, nOffset);
		pOut[5] = 0;
	}
	else
	{
		pOut[3] = 0;
		pOut[5] = RdI32(g_d2common_dat_6fded180, nOffset);
	}
	pOut[9] = RdI32(g_d2common_dat_6fded180, nOffset + 0x8);
	pOut[10] = RdI32(g_d2common_dat_6fded180, nOffset + 0xC);
	if (RdI32(g_d2common_dat_6fded180, nOffset + 0x4) == 0)
		*reinterpret_cast<uint16_t*>(reinterpret_cast<uint8_t*>(pOut) + 0x38) = 1;
	pOut[8] += RdI32(g_d2common_dat_6fded180, nOffset + 0x14);
	pOut[0xD] += RdI32(g_d2common_dat_6fded180, nOffset + 0x10);

	// Table D: stride 0x14 (item 0x0B, item 0x0C, item 4 flag when 0x0B is 9..11).
	nRoll = d2::rollLimited(seed, static_cast<int32_t>(g_d2common_dat_6fded090[nIndex]));
	nOffset = static_cast<int32_t>(nRoll + g_d2common_dat_6fded080[nIndex]) * 0x14;
	const int32_t nKind = RdI32(g_d2common_dat_6fded210, nOffset);
	pOut[0xB] = nKind;
	pOut[0xC] = RdI32(g_d2common_dat_6fded210, nOffset + 0xC);
	pOut[4] = (nKind >= 9 && nKind <= 0xB) ? 1 : 0;
	const int32_t nItem8Add = RdI32(g_d2common_dat_6fded210, nOffset + 0x8);
	const int32_t nItem8 = pOut[8];
	pOut[8] = nItem8 + nItem8Add;
	pOut[0xD] += RdI32(g_d2common_dat_6fded210, nOffset + 0x4);

	if (nClass != 0)
	{
		const int32_t nBonus = static_cast<int32_t>(nClass) * 0x1C;
		pOut[6] += RdI32(g_d2common_dat_6fded300, nBonus);
		pOut[7] += RdI32(g_d2common_dat_6fded300, nBonus + 0x4);
		pOut[8] = nItem8 + nItem8Add + RdI32(g_d2common_dat_6fded300, nBonus + 0x8);
		pOut[9] = RdI32(g_d2common_dat_6fded300, nBonus + 0xC) * pOut[9];
		pOut[10] = RdI32(g_d2common_dat_6fded300, nBonus + 0x10) * pOut[10];
		pOut[0xC] += RdI32(g_d2common_dat_6fded300, nBonus + 0x14);
		pOut[0xD] = RdI32(g_d2common_dat_6fded300, nBonus + 0x18) * pOut[0xD];
	}
	return 1;
}
