// 1.13c D2Common+0x303E0 (ordinal 11103): records the used skill in the unit's skill list.
// Retail: a null unit is an assert and _exit(-1); here it returns. Otherwise, when the unit's
// skill list (unit+0xA8) is set, the list's used-skill slot receives the skill.
#include <emscripten.h>
#include "D2Skills.h"
#include "Units/Units.h"

extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_303E0_SetUsedSkill(D2UnitStrc* unit, D2SkillStrc* skill)
{
    if (!unit)
        return;
    if (unit->pSkills)
        unit->pSkills->pUsedSkill = skill;
}
