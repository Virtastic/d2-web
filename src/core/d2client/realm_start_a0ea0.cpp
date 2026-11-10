// 1.13c D2Client+0xA0EA0
// Client start function 1 (the plain Attack): a bow or crossbow needs ammunition; any other weapon
// starts unless it is of item type 38.
#include <D2Composit.h>
#include <D2Items.h>
#include <Units/Units.h>
#include "realm_skill_start.h"
int d2client_realm_a0ea0(D2UnitStrc* unit, int, int) {
    if (COMPOSIT_IsWeaponBowOrXBow(unit)) return d2client_realm_a0a30(unit) != 0;
    D2UnitStrc* weapon = D2Common_10434(unit, 0);
    return (weapon && ITEMS_CheckItemTypeId(weapon, 0x26)) ? 0 : 1;
}
