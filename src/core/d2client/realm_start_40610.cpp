// 1.13c D2Client+0x40610
// Client start function 10: starts when the unit's target is a monster or a player.
#include <D2DataTbls.h>
#include <D2Skills.h>
#include <D2States.h>
#include <Path/Path.h>
#include <Units/Units.h>
#include "realm_skill_start.h"
#include "../compat/test_seam.h"

int d2client_realm_40610(D2UnitStrc* unit, int, int) {
    D2_CALL(d2client_realm_2a0b0, unit);
    if (!unit->pDynamicPath) return 0;
    D2UnitStrc* target = D2_CALL(PATH_GetTargetUnit, unit->pDynamicPath);
    return target && (target->dwUnitType == UNIT_PLAYER || target->dwUnitType == UNIT_MONSTER);
}
