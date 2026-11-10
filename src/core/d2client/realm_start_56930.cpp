// 1.13c D2Client+0x56930
// Client start function 40: stores the skill's first client calc on the used skill.
#include <D2DataTbls.h>
#include <D2Skills.h>
#include <D2States.h>
#include <Path/Path.h>
#include <Units/Units.h>
#include "realm_skill_start.h"

int d2client_realm_56930(D2UnitStrc* unit, int skillId, int level) {
    D2SkillsTxt* row = DATATBLS_GetSkillsTxtRecord(skillId);
    if (!row) return 0;
    D2SkillStrc* used = UNITS_GetUsedSkill(unit);
    if (!used) return 0;
    SKILLS_SetParam1(used, SKILLS_EvaluateSkillFormula(unit, row->dwCltCalc[0], skillId, level));
    return 1;
}
