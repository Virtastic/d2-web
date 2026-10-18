// 1.13c D2Common implementation batch: +0x4D840, +0x4E900, +0x3B080, +0x245D0.
// Bodies follow the cached 1.13c decompile. Callees that are still separate UNMAPPED rows arrive
// through an env struct passed after the retail arguments (a null env leaves the call inert);
// retail's fatal paths (Fog message + _exit(-1)) are modelled as returns, as in the other
// d2common batch files.
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "D2DataTbls.h"

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t A(const void* p) { return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p)); }
	inline uint32_t& W(uint8_t* b, int o) { return *reinterpret_cast<uint32_t*>(b + o); }
	inline uint32_t R(const uint8_t* b, int o) { return *reinterpret_cast<const uint32_t*>(b + o); }
}

// 0x4D840's only callee is retail FUN_6fd9d140 (RVA 0x4D140), the FieldTbls expanding-square
// search: (context, in/out x/y, optional match block, flags, radius, step).
struct D2Common_10509_Env
{
	int32_t* (*pFindFieldRecord)(uint32_t, int32_t*, uint32_t*, uint32_t, uint32_t, uint32_t,
	                             int32_t, int32_t, int32_t);
};

// 1.13c D2Common+0x4D840: search for a field-table record around (x,y) with radius 50 and step 1.
// The optional match block is null, the caller's fourth argument is passed as both flag words and
// the fifth as the "allow untouched cell" answer.
extern "C"
void D2Common_10509_FindFieldRecordNearby(uint32_t nContext, int32_t* pCoords, uint32_t nFlags,
                                          uint32_t nMatchFlags, int32_t nAllowUntouched,
                                          const D2Common_10509_Env* pEnv)
{
	if (pEnv != nullptr && pEnv->pFindFieldRecord != nullptr)
	{
		pEnv->pFindFieldRecord(nContext, pCoords, nullptr, nFlags, nMatchFlags, nMatchFlags,
		                       nAllowUntouched, 0x32, 1);
	}
}

// 0x4E900's skill-calc evaluator: retail FUN_6fd9d140's sibling, the Fog AST runner at RVA
// 0x9270 (Fog ordinal 10253). Arguments: code blob, byte length, two fixed callbacks, a data
// word and the three-int context { owner, skill id, level }; non-zero return is the percentage.
struct D2Common_10510_Env
{
	int32_t (*pEvaluateSkillCalc)(uint8_t*, uint32_t, void*, void*, void*, const int32_t*);
};

// 1.13c D2Common+0x4E900: skills.txt elemental length for nSkillId at nLevel.
// Row nSkillId of the 0x23C-byte skills table: levels 1..8 add dwELevLen[0] each, 9..16 add
// dwELevLen[1] on top of seven dwELevLen[0], 17+ add dwELevLen[2] on top of eight dwELevLen[1]
// and seven dwELevLen[0]. When dwELenSymPerCalc is not -1 and lies inside the skill-calc blob,
// the AST evaluator yields a percentage that is applied to the value and added again.
extern "C"
int32_t D2Common_10510_GetSkillElementalLength(int32_t nContext, int32_t nSkillId, int32_t nLevel,
                                               const D2Common_10510_Env* pEnv)
{
	if (sgptDataTables == nullptr || sgptDataTables->pSkillsTxt == nullptr)
		return 0;
	if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount)
		return 0;
	if (nLevel <= 0)
		return 0;

	const D2SkillsTxt* pRow = &sgptDataTables->pSkillsTxt[nSkillId];
	int32_t nOffset;
	if (nLevel < 0x11)
	{
		if (nLevel < 9)
			nOffset = (nLevel - 1) * static_cast<int32_t>(pRow->dwELevLen[0]);
		else
			nOffset = (nLevel - 8) * static_cast<int32_t>(pRow->dwELevLen[1]) +
			          7 * static_cast<int32_t>(pRow->dwELevLen[0]);
	}
	else
	{
		nOffset = (nLevel - 0x10) * static_cast<int32_t>(pRow->dwELevLen[2]) +
		          8 * static_cast<int32_t>(pRow->dwELevLen[1]) +
		          7 * static_cast<int32_t>(pRow->dwELevLen[0]);
	}

	int32_t nValue = static_cast<int32_t>(pRow->dwELen) + nOffset;
	const uint32_t nCalc = pRow->dwELenSymPerCalc;

	if (nCalc != 0xFFFFFFFFu && sgptDataTables->pSkillsCode != nullptr &&
	    nCalc < sgptDataTables->nSkillsCodeSize && pEnv != nullptr &&
	    pEnv->pEvaluateSkillCalc != nullptr)
	{
		const int32_t context[3] = { nContext, nSkillId, nLevel };
		const int32_t nPercent = pEnv->pEvaluateSkillCalc(
			reinterpret_cast<uint8_t*>(sgptDataTables->pSkillsCode) + nCalc,
			sgptDataTables->nSkillsCodeSize - nCalc, nullptr, nullptr, nullptr, context);
		if (nPercent != 0)
		{
			nValue += DATATBLS_ApplyRatio(nValue, nPercent, 100);
		}
	}
	return nValue;
}

// 1.13c D2Common+0x3B080: dword at +0x94 of the block whose address is at +0x48. A null struct
// or a null block answers 0.
extern "C"
uint32_t D2Common_10518_GetNestedDword0x94(const uint8_t* pStruct)
{
	if (pStruct == nullptr)
		return 0;
	const uint32_t nBlock = R(pStruct, 0x48);
	if (nBlock == 0)
		return 0;
	return R(P(nBlock), 0x94);
}

extern "C" uint32_t D2Common_22880_ItemGemApplyType(uint32_t nItemId);

// 1.13c D2Common+0x245D0: gem-apply type of the class id of a type-4 (item) unit.
// A null unit is fatal 0xF3E and another unit type fatal 0xF3F; both are modelled as returns.
extern "C"
void D2Common_10519_GetUnitGemApplyType(const int32_t* pUnit)
{
	if (pUnit == nullptr || pUnit[0] != 4)
		return;
	D2Common_22880_ItemGemApplyType(static_cast<uint32_t>(pUnit[1]));
}

// 0x243B0's callees: 0x23130 yields items.txt bytes +0x0A/+0x0B for a type-4 unit, 0x230F0
// yields the unit's items.txt row index. Both are still UNMAPPED rows.
struct D2Common_10523_Env
{
	void (*pGetItemTypeBytes)(const int32_t*, uint8_t*, uint8_t*);
	int32_t (*pItemTypeFromUnit)(const int32_t*);
};

// 1.13c D2Common+0x243B0: items.txt nBody (byte +0x09) of a type-4 unit's item type.
// The two type bytes must not both be zero, and the type index must be in range; otherwise 0.
extern "C"
uint8_t D2Common_10523_GetItemBody(const int32_t* pUnit, const D2Common_10523_Env* pEnv)
{
	uint8_t nType1 = 0;
	uint8_t nType2 = 0;
	if (pUnit == nullptr || pUnit[0] != 4 || pEnv == nullptr || pEnv->pGetItemTypeBytes == nullptr)
		return 0;
	pEnv->pGetItemTypeBytes(pUnit, &nType1, &nType2);
	if (nType1 == 0 && nType2 == 0)
		return 0;
	const int32_t nType = pEnv->pItemTypeFromUnit ? pEnv->pItemTypeFromUnit(pUnit) : -1;
	if (nType < 0 || sgptDataTables == nullptr || sgptDataTables->pItemTypesTxt == nullptr ||
	    nType >= sgptDataTables->nItemTypesTxtRecordCount)
		return 0;
	return sgptDataTables->pItemTypesTxt[nType].nBody;
}

// 1.13c D2Common+0x34880: byte store at +0x90 of the passed block.
extern "C"
void D2Common_10527_SetByte0x90(uint8_t* pBlock, uint8_t nValue)
{
	pBlock[0x90] = nValue;
}

// 1.13c D2Common+0x3BF20: when the block at +0x10 is in state 2, the address of a cell inside
// its layer: record at [[[+0x10]+0x20]+8], offset = dword[+4] * 0x3C + 0x44 + dword[+8].
// Otherwise a pointer to retail's static default record (DAT_6fde3338, contents not covered here).
extern "C"
uint8_t* D2Common_10529_GetLayerCell(int32_t* pUnit)
{
	static uint8_t g_D2Common_10529_default[8] = { 0 };
	if (pUnit == nullptr)
		return nullptr;
	const uint32_t nBlock = static_cast<uint32_t>(pUnit[4]);
	if (R(P(nBlock), 0x48) != 2)
		return g_D2Common_10529_default;
	const uint32_t nRecord = R(P(R(P(nBlock), 0x20)), 8);
	const int32_t nRow = *reinterpret_cast<const int32_t*>(P(nRecord) + 4);
	const int32_t nColumn = *reinterpret_cast<const int32_t*>(P(nRecord) + 8);
	return P(static_cast<uint32_t>(nRow * 0x3c + 0x44 + nColumn));
}

// 0x66800's callees: 0x665C0 refreshes the unit's room membership, 0x391F0 answers its
// alignment, and 0x66630 validates (room, unit, allow-already-present) — register-ordered in
// retail (ESI=room, EDI=unit, stack flag). All three are still UNMAPPED rows.
struct D2Common_10531_Env
{
	void (*pRefreshUnitInRoom)(int32_t*);
	int32_t (*pGetUnitAlignment)(int32_t*);
	uint32_t (*pValidateRoomUnit)(int32_t*, int32_t*, int32_t);
};

// 1.13c D2Common+0x66800: links the unit at the head of the room's unit list (room+0x74, unit
// next at +0xE8), refreshes it, and counts it in room+0x28 when it is a plain unit or a
// type-1 unit with alignment 2. Null unit/room (fatal 0x42/0x43) and a failed validation
// (fatal 0x4C, answer 0) yield 0.
extern "C"
uint32_t D2Common_10531_AddUnitToRoom(int32_t* pUnit, int32_t* pRoom,
                                      const D2Common_10531_Env* pEnv)
{
	if (pUnit == nullptr || pRoom == nullptr)
		return 0;
	if (pEnv == nullptr || pEnv->pValidateRoomUnit == nullptr)
		return 0;
	if (pEnv->pValidateRoomUnit(pRoom, pUnit, 1) == 0)
		return 0;

	pUnit[0x3a] = pRoom[0x1d];
	pRoom[0x1d] = static_cast<int32_t>(A(pUnit));
	if (pEnv->pRefreshUnitInRoom != nullptr)
		pEnv->pRefreshUnitInRoom(pUnit);

	if (pUnit[0] == 0)
	{
		pRoom[0x0a] += 1;
		return 1;
	}
	if (pUnit[0] != 1)
		return 1;
	if (pEnv->pGetUnitAlignment != nullptr && pEnv->pGetUnitAlignment(pUnit) == 2)
		pRoom[0x0a] += 1;
	return 1;
}

// 1.13c D2Common+0x225F0: stores a dword at +0x28 of a type-4 unit's unit data.
// Null unit (fatal 0x107D), wrong type (0x107E) and missing unit data (0x1080) are modelled as
// returns.
extern "C"
void D2Common_10536_SetItemDataDword28(int32_t* pUnit, uint32_t nValue)
{
	if (pUnit == nullptr || pUnit[0] != 4 || pUnit[5] == 0)
		return;
	W(P(static_cast<uint32_t>(pUnit[5])), 0x28) = nValue;
}

// 1.13c D2Common+0x22260: a value scaled by the class id of a type-0 (player) unit.
// Classes 0, 3 and 6 give (value >> 1) + value, any other class gives the value unchanged.
// A null unit or a non-player unit gives value * 2.
extern "C"
int32_t D2Common_10540_ScaleByUnitClass(int32_t* pUnit, int32_t nValue)
{
	if (pUnit == nullptr || pUnit[0] != 0)
		return nValue * 2;
	switch (pUnit[1])
	{
	case 0:
	case 3:
	case 6:
		return (nValue >> 1) + nValue;
	case 4:
		break;
	default:
		return nValue;
	}
	return nValue * 2;
}

// 1.13c D2Common+0x2F0C0: rewrites the dword at +0x38 as its old low byte plus nValue in the
// second byte. A null block is fatal 0xACE, modelled as a return.
extern "C"
void D2Common_10542_SetWordInDword0x38(uint8_t* pBlock, int32_t nValue)
{
	if (pBlock == nullptr)
		return;
	W(pBlock, 0x38) = (R(pBlock, 0x38) & 0xff) + static_cast<uint32_t>(nValue) * 0x100;
}

// 1.13c D2Common+0x24EB0: items.txt wShoots (word +0x0C) of a type-4 unit's item type.
// The type index comes from 0x230F0, still an UNMAPPED row; a null unit or a non-item yields 0.
extern "C"
int32_t D2Common_10543_GetItemShoots(const int32_t* pUnit, const D2Common_10523_Env* pEnv)
{
	if (pUnit == nullptr || pUnit[0] != 4)
		return 0;
	const int32_t nType = (pEnv != nullptr && pEnv->pItemTypeFromUnit)
	                          ? pEnv->pItemTypeFromUnit(pUnit)
	                          : -1;
	if (nType < 0 || sgptDataTables == nullptr || sgptDataTables->pItemTypesTxt == nullptr ||
	    nType >= sgptDataTables->nItemTypesTxtRecordCount)
		return 0;
	return static_cast<int16_t>(sgptDataTables->pItemTypesTxt[nType].wShoots);
}
