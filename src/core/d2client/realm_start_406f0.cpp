// 1.13c D2Client+0x406F0
// Client start function 6: stores the skill's first calc on the used skill when the unit has a target, else
// zero. Refuses for a bad skill id or without a used skill.
#include <D2DataTbls.h>
#include <D2Skills.h>
#include <D2States.h>
#include <Path/Path.h>
#include <Units/Units.h>
#include "realm_skill_start.h"

int d2client_realm_406f0(D2UnitStrc* unit, int skillId, int level) {
    D2SkillsTxt* row = DATATBLS_GetSkillsTxtRecord(skillId);
    if (!row) return 0;
    D2DynamicPathStrc* path = unit->pDynamicPath;
    if (!path) return 0;
    D2UnitStrc* target = PATH_GetTargetUnit(path);
    D2SkillStrc* used = UNITS_GetUsedSkill(unit);
    if (!used) return 0;
    if (!target) { SKILLS_SetParam1(used, 0); return 1; }
    SKILLS_SetParam1(used, SKILLS_EvaluateSkillFormula(unit, row->dwCalc[0], skillId, level));
    return 1;
}
