#pragma once

#include <Units/Units.h>

// Realm player CUnits are allocated independently of the offline session. Keep the native
// inventory attachment on that canonical unit so skill/mana/item helpers see the same owner.
extern "C" int d2_realm_client_unit_inventory_init(D2UnitStrc* unit);
extern "C" int d2_realm_client_unit_inventory_ready(const D2UnitStrc* unit);
extern "C" void d2_realm_client_unit_inventory_destroy(D2UnitStrc* unit);
