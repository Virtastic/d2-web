#include "d2gs_client_unit_inventory.h"

#include <D2Inventory.h>

extern "C" int d2_realm_client_unit_inventory_ready(const D2UnitStrc* unit) {
    if (!unit || unit->dwUnitType != UNIT_PLAYER || !unit->pInventory) return 0;
    const D2InventoryStrc* inventory = unit->pInventory;
    return inventory->dwSignature == D2C_InventoryHeader &&
        inventory->pOwner == unit && inventory->dwOwnerGuid == unit->dwUnitId;
}

extern "C" int d2_realm_client_unit_inventory_init(D2UnitStrc* unit) {
    if (!unit || unit->dwUnitType != UNIT_PLAYER) return 0;
    if (unit->pInventory) return d2_realm_client_unit_inventory_ready(unit);

    // This is the engine-side equivalent of D2Client 1.13c's player constructor calling
    // Common #10387. The helper installs the real D2InventoryStrc and owner back-reference.
    D2InventoryStrc* inventory = INVENTORY_AllocInventory(nullptr, unit);
    if (!inventory) return 0;
    if (!d2_realm_client_unit_inventory_ready(unit)) {
        INVENTORY_FreeInventory(inventory);
        return 0;
    }
    return 1;
}

extern "C" void d2_realm_client_unit_inventory_destroy(D2UnitStrc* unit) {
    if (!unit || !unit->pInventory) return;
    if (!d2_realm_client_unit_inventory_ready(unit)) return;

    // UNITS_FreeUnit intentionally does not own pInventory; release it first so its owner
    // back-reference is nulled while the CUnit is still alive.
    INVENTORY_FreeInventory(unit->pInventory);
}
