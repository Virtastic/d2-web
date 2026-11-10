// 1.13c D2Client+0xA0E70
// Client start function 2: starts with ammunition and a hand weapon.
#include <Units/Units.h>
#include "realm_skill_start.h"
int d2client_realm_a0e70(D2UnitStrc* unit, int, int) {
    if (!d2client_realm_a0a30(unit)) return 0;
    return D2Common_10434(unit, 0) != nullptr;
}
