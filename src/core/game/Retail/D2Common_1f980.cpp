// Moved from the stand-alone d2common batch into the game tree: can two items share the hand pair (ammo/quiver, two-handed, dual-wield classes).
#include <cstdint>
#include <emscripten.h>
#include "D2Items.h"
#include "D2Monsters.h"
#include "D2DataTbls.h"
#include "Units/Units.h"
static inline uint32_t D2Common_24430_CheckItemTypeId(const D2UnitStrc* pItem, int nItemType) { return ITEMS_CheckItemTypeId(pItem, nItemType); }   // the game's 0x24430 body

// 1.13c D2Common+0x1F980
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_port_d2common_1f980(D2UnitStrc* unit, D2UnitStrc* first, D2UnitStrc* second)
{
    // Item-type tests are the 1.13c 0x24430 form (D2Common_24430_CheckItemTypeId), not the 1.10f ITEMS_CheckItemTypeId.
    if (!first || !second) return 1;
    if (ITEMS_GetAmmoType(first) && D2Common_24430_CheckItemTypeId(second, ITEMS_GetAmmoType(first))) return 1;
    if (ITEMS_GetAmmoType(second) && D2Common_24430_CheckItemTypeId(first, ITEMS_GetAmmoType(second))) return 1;
    if (ITEMS_GetQuiverType(first) || ITEMS_GetQuiverType(second)) return 0;
    if (ITEMS_CheckWeaponIfTwoHanded(first) && !ITEMS_Is1Or2Handed(unit, first)) return 0;
    if (ITEMS_CheckWeaponIfTwoHanded(second) && !ITEMS_Is1Or2Handed(unit, second)) return 0;
    if (!D2Common_24430_CheckItemTypeId(first, 0x2d) || !D2Common_24430_CheckItemTypeId(second, 0x2d))
        return D2Common_24430_CheckItemTypeId(first, 0x2d) || D2Common_24430_CheckItemTypeId(second, 0x2d);
    if (!unit) return 0;
    if (unit->dwUnitType == 0) {
        if (unit->dwClassId == 4) return 1;
        if (unit->dwClassId != 6) return 0;
    } else if (unit->dwUnitType == 1) {
        switch (MONSTERS_ValidateMonsterId(int32_t(unit->dwClassId))) {
            case 0x21c: case 0x21d: case 0x21e: return 1;
            case 0x1a1: case 0x1a2: break;
            default: return 0;
        }
    } else return 0;
    return D2Common_24430_CheckItemTypeId(first, 0x43) && D2Common_24430_CheckItemTypeId(second, 0x43);
}
