#include "d2gs_client_unit_resources.h"

#include "d2gs_client_unit_gfx.h"
#include "d2gs_client_unit_inventory.h"

namespace {
bool supported_unit(const D2UnitStrc* unit) {
    return unit && (unit->dwUnitType == UNIT_PLAYER || unit->dwUnitType == UNIT_MONSTER);
}
}

extern "C" int d2_realm_client_unit_resources_attached(const D2UnitStrc* unit) {
    if (!supported_unit(unit) || !d2_realm_client_unit_gfx_record_valid(unit)) return 0;
    return unit->dwUnitType != UNIT_PLAYER || d2_realm_client_unit_inventory_ready(unit);
}

extern "C" int d2_realm_client_unit_resources_status(const D2UnitStrc* unit) {
    if (!supported_unit(unit)) return 0;
    int status = 1;
    if (unit->pGfxData) status |= 1 << 1;
    if (d2_realm_client_unit_gfx_record_valid(unit)) status |= 1 << 2;
    if (d2_realm_client_unit_gfx_empty_record(unit)) status |= 1 << 3;
    if (unit->dwUnitType == UNIT_PLAYER && d2_realm_client_unit_inventory_ready(unit)) status |= 1 << 4;
    if (d2_realm_client_unit_resources_attached(unit)) status |= 1 << 5;
    return status;
}

extern "C" int d2_realm_client_unit_resources_init(D2UnitStrc* unit) {
    if (!supported_unit(unit)) return 0;

    const bool hadInventory = unit->pInventory != nullptr;
    if (unit->dwUnitType == UNIT_PLAYER && !d2_realm_client_unit_inventory_init(unit)) return 0;
    if (!d2_realm_client_unit_gfx_init(unit)) {
        if (unit->dwUnitType == UNIT_PLAYER && !hadInventory)
            d2_realm_client_unit_inventory_destroy(unit);
        return 0;
    }
    return d2_realm_client_unit_resources_attached(unit);
}

extern "C" void d2_realm_client_unit_resources_destroy(D2UnitStrc* unit) {
    if (!supported_unit(unit)) return;
    d2_realm_client_unit_gfx_destroy(unit);
    if (unit->dwUnitType == UNIT_PLAYER) d2_realm_client_unit_inventory_destroy(unit);
}
