// D2Common implementation batch: +0x67FB0, +0x22170, +0x32040, +0x529E0 translated from the pinned
// 1.13c decompiles. Callees that are still UNMAPPED rows are seam function pointers passed after the
// retail arguments; the vendored 1.13c-checked helpers (0x4ED60, 0x52660) are called by name.
#include <cstddef>
#include <cstdint>

#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Skills.h"
#include "Units/Units.h"

// 0x52660 AddSkill, implemented in src/core/d2common/d2common_batch_52660.cpp.
D2SkillStrc* D2Common_52660_AddSkill(D2UnitStrc* pUnit, int32_t nSkillId);

// 1.13c D2Common+0x67FB0 (#10502): three pointer hops from the argument (+0x10, then +0x54) and the
// dword at +0x10 of the last. The argument's struct is not identified (no callers in the image); the
// offsets are kept raw. A null argument is 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_67fb0(const void* pArg)
{
	if (!pArg) return 0;

	const uint8_t* pFirst = *(const uint8_t* const*)((const uint8_t*)pArg + 0x10);
	if (!pFirst) return 0;

	const uint8_t* pSecond = *(const uint8_t* const*)(pFirst + 0x54);
	if (!pSecond) return 0;

	return *(const uint32_t*)(pSecond + 0x10);
}

// 1.13c D2Common+0x22170 (#10503): write the item data's wItemFormat (+0x30). A null unit (0x1510),
// a non-item unit (0x1511) and a missing item data (0x1513) are fatal in retail; we return.
extern "C"
EMSCRIPTEN_KEEPALIVE void ITEMS_SetItemFormat(D2UnitStrc* pUnit, uint16_t nItemFormat)
{
	if (!pUnit) return;
	if (pUnit->dwUnitType != UNIT_ITEM) return;
	if (!pUnit->pItemData) return;

	pUnit->pItemData->wItemFormat = nItemFormat;
}

namespace
{
	// Callees of 0x32040 that still own an UNMAPPED row: 0x38A80 (total stat value of one
	// itemstatcost row for a stat list, STATLIST_GetTotalStat_6FDB63E0) is mapped but its retail
	// operand order is register-only, so it is taken through this seam.
	struct Batch3_32040Env
	{
		int32_t (*getTotalStat)(D2StatListExStrc* pStatList, int32_t nItemStatCostTxtRow); // 0x38A80
	};

	// One itemstatcost row value, or 0 when the row is missing, the unit has no stat list or the
	// seam is not wired.
	int32_t ReadItemStatCostValue(D2UnitStrc* pUnit, int32_t nRow, const Batch3_32040Env* pEnv)
	{
		if (!pUnit || !pUnit->pStatListEx) return 0;
		if (nRow < 0 || nRow >= sgptDataTables->nItemStatCostTxtRecordCount) return 0;
		if (!sgptDataTables->pItemStatCostTxt) return 0;
		if (!pEnv || !pEnv->getTotalStat) return 0;
		return pEnv->getTotalStat(pUnit->pStatListEx, nRow);
	}
}

// 1.13c D2Common+0x32040 (#10504): the per-level regeneration penalty from the itemstatcost rows
// 7 (max life), 9 (max mana), 6 (life), 8 (mana) and 12 (level): 0 when both missing pools are 0
// after an 8-bit shift, otherwise ((missingHP >> 8) + (missingMana >> 8)) * row12 >> 2.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t UNIT_GetLevelRegenPenalty_32040(D2UnitStrc* pUnit, const Batch3_32040Env* pEnv)
{
	const int32_t nMaxLife = ReadItemStatCostValue(pUnit, 7, pEnv);
	const int32_t nMaxMana = ReadItemStatCostValue(pUnit, 9, pEnv);
	const int32_t nLife = ReadItemStatCostValue(pUnit, 6, pEnv);
	const int32_t nMana = ReadItemStatCostValue(pUnit, 8, pEnv);

	uint32_t nLifeGap = 0;
	uint32_t nManaGap = 0;
	if ((uint32_t)nLife < (uint32_t)nMaxLife) nLifeGap = ((uint32_t)nMaxLife - (uint32_t)nLife) >> 8;
	if ((uint32_t)nMana < (uint32_t)nMaxMana) nManaGap = ((uint32_t)nMaxMana - (uint32_t)nMana) >> 8;

	if (nLifeGap == 0 && nManaGap == 0) return 0;

	const int32_t nLevel = ReadItemStatCostValue(pUnit, 12, pEnv);
	return ((nManaGap + nLifeGap) * (uint32_t)nLevel) >> 2;
}

// 1.13c D2Common+0x529E0 (#10506): give a player unit its class base skills. Unless the skill list
// already holds skill 0 with owner -1, add skill 0 and the ten nBaseSkill entries of the charstats
// row (guarded by the skills.txt count), then make sure the left and right active skills are set.
// A null skill list after the adds is fatal in retail; we return.
extern "C"
EMSCRIPTEN_KEEPALIVE void D2Common_529E0_InitBaseSkills(D2UnitStrc* pUnit)
{
	if (!pUnit) return;

	bool bHasNativeSkill = false;
	for (D2SkillStrc* pSkill = pUnit->pSkills ? pUnit->pSkills->pFirstSkill : nullptr; pSkill; pSkill = pSkill->pNextSkill)
	{
		if (pSkill->pSkillsTxt && pSkill->pSkillsTxt->nSkillId == 0 && pSkill->nOwnerGUID == 0xFFFFFFFF)
		{
			bHasNativeSkill = true;
			break;
		}
	}

	if (!bHasNativeSkill)
	{
		const int32_t nClassId = pUnit->dwClassId;
		if (nClassId < 0 || nClassId >= sgptDataTables->nCharStatsTxtRecordCount) return;
		if (!sgptDataTables->pCharStatsTxt) return;

		const D2CharStatsTxt* pRow = &sgptDataTables->pCharStatsTxt[nClassId];

		D2Common_52660_AddSkill(pUnit, 0);
		for (int32_t i = 0; i < 10; ++i)
		{
			const int32_t nSkillId = pRow->nBaseSkill[i];
			if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount) continue;
			D2Common_52660_AddSkill(pUnit, nSkillId);
		}
	}

	// list is re-read: AddSkill may have created it. Null here is fatal in retail (0x3EA/0x40F).
	D2SkillListStrc* pSkillList = pUnit->pSkills;
	if (!pSkillList) return;
	if (!pSkillList->pLeftSkill) SKILLS_SetLeftActiveSkill(pUnit, 0, -1);
	if (pUnit->pSkills && !pUnit->pSkills->pRightSkill) SKILLS_SetRightActiveSkill(pUnit, 0, -1);
}
