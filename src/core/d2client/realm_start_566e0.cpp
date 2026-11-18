// 1.13c D2Client+0x566E0
// Client start function 41: zeroes the used skill's flags. Refuses without a used skill.
#include <D2DataTbls.h>
#include <D2Skills.h>
#include <D2States.h>
#include <Path/Path.h>
#include <Units/Units.h>
#include "realm_skill_start.h"
#include "../d2moo/compat/test_seam.h"

int d2client_realm_566e0(D2UnitStrc* unit, int, int) {
    D2SkillStrc* used = D2_CALL(UNITS_GetUsedSkill, unit);
    if (!used) return 0;
    D2_CALL(SKILLS_SetFlags, used, 0);
    return 1;
}
