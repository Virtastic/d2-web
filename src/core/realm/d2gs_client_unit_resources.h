#pragma once

#include <Units/Units.h>

// Canonical lifecycle for client-side resources owned by realm actor CUnits.
extern "C" int d2_realm_client_unit_resources_init(D2UnitStrc* unit);
extern "C" int d2_realm_client_unit_resources_attached(const D2UnitStrc* unit);
// Bit 0: supported unit; 1: Gfx pointer; 2: native Gfx header; 3: empty 0x804-byte Gfx
// record except its two initialized dwords; 4: valid player inventory; 5: required resources ready.
extern "C" int d2_realm_client_unit_resources_status(const D2UnitStrc* unit);
extern "C" void d2_realm_client_unit_resources_destroy(D2UnitStrc* unit);
