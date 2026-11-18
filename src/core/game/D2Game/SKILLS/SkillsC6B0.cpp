#include "SKILLS/SkillDruid.h"

#include <test_seam.h>
#include <D2DataTbls.h>
#include <D2Skills.h>

#include "SKILLS/Skills.h"

// Written from the 1.13c decompile: no 1.10f counterpart. A valid skills.txt row gets a target-state curse
// (stat -1, no value, no remove callback) lasting the row's aura length; when the curse creates a stat list,
// the druid state routine fills it.
// 1.13c D2Game+0xC6B0
void __fastcall SKILLS_ApplySkillCurse(D2UnitStrc* pUnit, D2UnitStrc* pTarget, int32_t nSkillId, int32_t nSkillLevel)
{
    if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount)
    {
        return;
    }

    D2SkillsTxt* pSkillsTxtRecord = &sgptDataTables->pSkillsTxt[nSkillId];
    if (!pSkillsTxtRecord)
    {
        return;
    }

    D2CurseStrc curse = {};
    curse.pUnit = pUnit;
    curse.pTarget = pTarget;
    curse.nSkill = nSkillId;
    curse.nSkillLevel = nSkillLevel;
    curse.nDuration = D2_CALL(SKILLS_EvaluateSkillFormula, pUnit, pSkillsTxtRecord->dwAuraLenCalc, nSkillId, nSkillLevel);
    curse.nStat = -1;
    curse.nStatValue = 0;
    curse.nState = pSkillsTxtRecord->wAuraTargetState;

    D2StatListStrc* pStatList = D2_CALL(sub_6FD10EC0, &curse);
    if (pStatList)
    {
        D2_CALL(sub_6FCFE0E0, pUnit, pStatList, pSkillsTxtRecord, nSkillId, nSkillLevel);
    }
}
