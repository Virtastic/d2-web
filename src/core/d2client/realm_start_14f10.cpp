// 1.13c D2Client+0x14F10
// Client start function 12: clears the used skill's first parameter. Refuses without a used skill.
#include <D2Skills.h>
#include <Units/Units.h>
#include "realm_skill_start.h"
int d2client_realm_14f10(D2UnitStrc* unit, int, int) {
    D2SkillStrc* used = unit ? UNITS_GetUsedSkill(unit) : nullptr;
    if (!used) return 0;
    SKILLS_SetParam1(used, 0);
    return 1;
}
