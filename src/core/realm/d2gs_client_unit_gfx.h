#pragma once

#include <Units/Units.h>

// Empty client-side GfxData storage for a realm-owned CUnit. Actual DCC/COF resources remain
// owned by the browser renderer, not by this compatibility record.
extern "C" int d2_realm_client_unit_gfx_init(D2UnitStrc* unit);
extern "C" int d2_realm_client_unit_gfx_record_valid(const D2UnitStrc* unit);
extern "C" int d2_realm_client_unit_gfx_empty_record(const D2UnitStrc* unit);
extern "C" void d2_realm_client_unit_gfx_destroy(D2UnitStrc* unit);
