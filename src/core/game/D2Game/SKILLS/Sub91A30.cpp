#include <D2Game/oc20g11.h>

#include <algorithm>

#include <D2DataTbls.h>
#include <D2Skills.h>
#include <D2StatList.h>
#include <D2Combat.h>
#include <DataTbls/SkillsIds.h>
#include <test_seam.h>

#include "SKILLS/Skills.h"

// 1.13c D2Game+0x91A30 (stdcall: unit, stat list of the progressive state, damage)
// Case 3 of the progressive-damage switch (0x91E30): the stat list names the skill (stat 0x15E) and a level (0x15F), raised to the unit's
// own level of that skill. When the skill's aura stat reads back above 0 the elemental damage is rolled; a cold skill at state value 2..3
// adds coldLen / param[1] to the freeze length. Either way the hit is flagged 0x4000.
void __stdcall sub_91A30(D2UnitStrc* pUnit, D2StatListStrc* pStatList, D2DamageStrc* pDamage)
{
    const int32_t nSkillId = D2_CALL(STATLIST_GetStatValue, pStatList, STAT_MODIFIERLIST_SKILL, 0);
    if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount)
    {
        return;
    }

    D2SkillsTxt* pSkillsTxtRecord = &sgptDataTables->pSkillsTxt[nSkillId];
    if (!pSkillsTxtRecord)
    {
        return;
    }

    int32_t nSkillLevel = D2_CALL(STATLIST_GetStatValue, pStatList, STAT_MODIFIERLIST_LEVEL, 0);
    const int32_t nMaxSkillLevel = D2_CALL(SKILLS_GetSkillLevel, pUnit, D2_CALL(SKILLS_GetHighestLevelSkillFromUnitAndId, pUnit, nSkillId), 1);
    if (nMaxSkillLevel > nSkillLevel)
    {
        nSkillLevel = nMaxSkillLevel;
    }

    if (nSkillLevel <= 0 || pSkillsTxtRecord->wAuraStat[0] < 0 || pSkillsTxtRecord->wAuraStat[0] >= sgptDataTables->nItemStatCostTxtRecordCount)
    {
        return;
    }

    const int32_t nValue = D2_CALL(STATLIST_GetStatValue, pStatList, pSkillsTxtRecord->wAuraStat[0], 0);
    if (nValue <= 0)
    {
        return;
    }

    D2_CALL(D2GAME_RollElementalDamage_6FD14DD0, pUnit, pDamage, nSkillId, nSkillLevel);
    if (pSkillsTxtRecord->nEType == ELEMTYPE_COLD && nValue >= 2 && nValue <= 3 && pSkillsTxtRecord->dwParam[1])
    {
        pDamage->dwFrzLen += pDamage->dwColdLen / pSkillsTxtRecord->dwParam[1];
    }

    pDamage->wResultFlags |= 0x4000u;
}
