// 1.13c D2Common+0x1FEF0 (INVENTORY_HasBodyArmorEquipped, retail body). Signature check, grid 0 (body locs, 0x1E1B0
// with the existing grid returned, no allocation), torso cell ppItems[BODYLOC_TORSO] (index 3), then the retail item-type
// check for ITEMTYPE_ARMOR (3) at 0x24430. The 1.10f body in d2moo/src/D2Inventory.cpp calls the 1.10f ITEMS_CheckItemTypeId
// instead; this port is the 1.13c one for the wire-in.
#include <cstdint>
#include "D2Inventory.h"
#include "D2Items.h"

uint32_t D2Common_24430_CheckItemTypeId(const D2UnitStrc* pItem, int nItemType);
extern D2InventoryGridInfoStrc gBodyLocInventoryGridInfo;

static_assert(D2C_InventoryHeader == 0x01020304, "inventory signature (retail +0x00)");
static_assert(BODYLOC_TORSO == 3, "retail reads ppItems[3]");

uint32_t D2Common_1FEF0_HasBodyArmorEquipped(D2InventoryStrc* pInventory)
{
    if (pInventory && INVENTORY_CheckSignature(pInventory))
    {
        D2InventoryGridStrc* pGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
        D2UnitStrc* pTorso = pGrid ? pGrid->ppItems[BODYLOC_TORSO] : nullptr;
        if (pTorso && D2Common_24430_CheckItemTypeId(pTorso, ITEMTYPE_ARMOR)) return 1;
    }
    return 0;
}
