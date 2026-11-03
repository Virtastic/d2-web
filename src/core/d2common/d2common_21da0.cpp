// 1.13c D2Common+0x21da0: places the skill item into inventory grid 0 at the skill index (stdcall).
// Retail gates on a non-null inventory and 0 <= skill < 13; otherwise it returns 0.
#include <cstdint>
#include "D2Inventory.h"
#include "Units/Units.h"

uint32_t D2Common_21DA0(D2InventoryStrc* pInv, D2UnitStrc* pItem, int32_t nSkill) {
    if (pInv && nSkill >= 0 && nSkill < 13) {
        return INVENTORY_PlaceItemInGrid(pInv, pItem, nSkill, 0, 0, 0, 0);
    }
    return 0;
}
