// 1.13c D2Common implementation batch: twelve rows (0x1610..0x18A0) translated from their pinned
// decompiles. Skills.txt dwParam getters, the ItemStatCost/Sets/SetItems/UniqueItems row getters,
// a monster's base id and a monster-flags bit. Table strides are retail's 32-bit record sizes.
#include <cstddef>
#include <cstdint>

#include "D2BitManip.h"
#include "D2DataTbls.h"
#include "DataTbls/ItemsTbls.h"
#include "DataTbls/MonsterTbls.h"
#include "DataTbls/SkillsTbls.h"
#include "Units/Units.h"

static_assert(sizeof(D2SkillsTxt) == 0x23C, "skills.txt record stride");
static_assert(sizeof(D2ItemStatCostTxt) == 0x144, "itemstatcost.txt record stride");
static_assert(sizeof(D2SetsTxt) == 0x128, "sets.txt record stride");
static_assert(sizeof(D2SetItemsTxt) == 0x1B8, "setitems.txt record stride");
static_assert(sizeof(D2UniqueItemsTxt) == 0x14C, "uniqueitems.txt record stride");
static_assert(sizeof(D2MonStatsTxt) == 0x1A8, "monstats.txt record stride");

namespace
{
	// Retail's guard is the computed row address against zero, not the table pointer; a skill id in
	// range with a null table would fault in retail too. Uintptr arithmetic keeps that read exact.
	inline const D2SkillsTxt* SkillRow(int32_t nSkillId)
	{
		if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount) return nullptr;
		const uintptr_t nRow = (uintptr_t)sgptDataTables->pSkillsTxt + (uintptr_t)nSkillId * sizeof(D2SkillsTxt);
		return nRow != 0 ? (const D2SkillsTxt*)nRow : nullptr;
	}

	// (level - 1) * high + low for the dwParam pair at byte offsets nLow/nHigh; level <= 0 gives 0.
	inline int32_t SkillLinear(int32_t nSkillId, int32_t nLevel, size_t nLow, size_t nHigh)
	{
		const D2SkillsTxt* pRow = SkillRow(nSkillId);
		if (!pRow || nLevel <= 0) return 0;
		const int32_t* pParam = pRow->dwParam;
		return (nLevel - 1) * pParam[nHigh / sizeof(int32_t)] + pParam[nLow / sizeof(int32_t)];
	}
}

// 1.13c D2Common+0x1610
int32_t D2Common_1610_GetSkillParamDword4(int32_t nSkillId)
{
	const D2SkillsTxt* pRow = SkillRow(nSkillId);
	return pRow ? pRow->dwParam[4] : 0;
}

// 1.13c D2Common+0x1640
int32_t D2Common_1640_GetSkillParamDword5(int32_t nSkillId)
{
	const D2SkillsTxt* pRow = SkillRow(nSkillId);
	return pRow ? pRow->dwParam[5] : 0;
}

// 1.13c D2Common+0x1670
int32_t D2Common_1670_GetSkillParamLinear01(int32_t nSkillId, int32_t nLevel)
{
	return SkillLinear(nSkillId, nLevel, 0x148, 0x14C);
}

// 1.13c D2Common+0x16B0
int32_t D2Common_16B0_GetSkillParamLinear23(int32_t nSkillId, int32_t nLevel)
{
	return SkillLinear(nSkillId, nLevel, 0x150, 0x154);
}

// 1.13c D2Common+0x16F0
int32_t D2Common_16F0_GetSkillParamLinear45(int32_t nSkillId, int32_t nLevel)
{
	return SkillLinear(nSkillId, nLevel, 0x158, 0x15C);
}

// 1.13c D2Common+0x1730
int32_t D2Common_1730_GetSkillParamLinear67(int32_t nSkillId, int32_t nLevel)
{
	return SkillLinear(nSkillId, nLevel, 0x160, 0x164);
}

// 1.13c D2Common+0x17A0
D2ItemStatCostTxt* D2Common_17A0_GetItemStatCostTxtRecord(int32_t nStatId)
{
	if (nStatId < 0 || nStatId >= sgptDataTables->nItemStatCostTxtRecordCount) return nullptr;
	return (D2ItemStatCostTxt*)((uintptr_t)sgptDataTables->pItemStatCostTxt
		+ (uintptr_t)nStatId * sizeof(D2ItemStatCostTxt));
}

// 1.13c D2Common+0x17D0
D2SetsTxt* D2Common_17D0_GetSetsTxtRecord(int32_t nSetId)
{
	return DATATBLS_GetSetsTxtRecord(nSetId);
}

// 1.13c D2Common+0x1800
D2SetItemsTxt* D2Common_1800_GetSetItemsTxtRecord(int32_t nSetItemId)
{
	return DATATBLS_GetSetItemsTxtRecord(nSetItemId);
}

// 1.13c D2Common+0x1830
D2UniqueItemsTxt* D2Common_1830_GetUniqueItemsTxtRecord(int32_t nUniqueItemId)
{
	if (nUniqueItemId < 0 || nUniqueItemId >= sgptDataTables->nUniqueItemsTxtRecordCount) return nullptr;
	return (D2UniqueItemsTxt*)((uintptr_t)sgptDataTables->pUniqueItemsTxt
		+ (uintptr_t)nUniqueItemId * sizeof(D2UniqueItemsTxt));
}

// 1.13c D2Common+0x1860
int32_t D2Common_1860_GetMonsterBaseId(const D2UnitStrc* pUnit)
{
	if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER) return -1;
	const int32_t nClassId = pUnit->dwClassId;
	const D2MonStatsTxt* pRow = DATATBLS_GetMonStatsTxtRecord(nClassId);
	return pRow ? pRow->nBaseId : nClassId;
}

// 1.13c D2Common+0x18A0
uint32_t D2Common_18A0_GetMonStatsFlagBit(int32_t nMonsterId, uint32_t nBit)
{
	const D2MonStatsTxt* pRow = DATATBLS_GetMonStatsTxtRecord(nMonsterId);
	if (!pRow) return 0;
	return pRow->nMonStatsFlags[nBit >> 3] & gdwBitMasks[nBit & 7];
}

// 1.13c D2Common+0x18F0 (skill id arrives in EAX): 1 if the skill's wSkillDesc (+0x194) indexes a valid SkillDesc row
int32_t D2Common_18F0_SkillHasValidDesc(int32_t nSkillId)
{
	if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount) return 0;
	const uint8_t* pRec = (const uint8_t*)sgptDataTables->pSkillsTxt + (uintptr_t)nSkillId * 0x23C;
	if (!pRec) return 0;
	const int32_t nDesc = *(const int16_t*)(pRec + 0x194);
	return nDesc >= 0 && nDesc < sgptDataTables->nSkillDescTxtRecordCount;
}
