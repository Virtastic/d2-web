// 1.13c D2Client+0x57000
// Client start function 39: needs a target unit on the path; clears the unit's 0x40000 extended flag.
#include <Path/Path.h>
#include <Units/Units.h>
#include "realm_skill_start.h"
int d2client_realm_57000(D2UnitStrc* unit, int, int) {
    if (!unit || !unit->pDynamicPath || !PATH_GetTargetUnit(unit->pDynamicPath)) return 0;
    unit->dwFlagEx &= ~0x40000u;
    return 1;
}
