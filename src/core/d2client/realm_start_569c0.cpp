// 1.13c D2Client+0x569C0
// Client start function 37: clears the unit's three low flag bits (0xE).
#include <D2DataTbls.h>
#include <D2Skills.h>
#include <D2States.h>
#include <Path/Path.h>
#include <Units/Units.h>
#include "realm_skill_start.h"

int d2client_realm_569c0(D2UnitStrc* unit, int, int) {
    if (unit) unit->dwFlags &= 0xfffffff1u;
    return 1;
}
