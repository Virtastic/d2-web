// 1.13c D2Common+0x21ee0: belt-slot fit test for an item (stdcall: inventory, item, slot).
// Item type 4, beltable per ITEMS_CheckIfBeltable, a 1x1 record, slot 0..15: placed in the belt grid (grid 1).
// Retail's record-less fallback compares the item pointer's low byte; that is not reproducible, so fixtures
// always carry a record.
#include <cstdint>
#include "D2Inventory.h"
#include "D2Items.h"
#include "DataTbls/ItemsTbls.h"
#include "Units/Units.h"

uint32_t D2Common_21EE0(D2InventoryStrc* pInv, D2UnitStrc* pItem, int32_t nSlot) {
    if (!pItem || pItem->dwUnitType != 4) {
        return 0;
    }
    if (ITEMS_CheckIfBeltable(pItem) == 0) {
        return 0;
    }
    const D2ItemsTxt* pRec = DATATBLS_GetItemsTxtRecord((int)pItem->dwClassId);
    uint8_t nW = 0;
    uint8_t nH = 0;
    if (pRec) {
        nW = pRec->nInvWidth;
        nH = pRec->nInvHeight;
    } else {
        nW = nH = (uint8_t)(uintptr_t)pItem;
    }
    if (nW == 1 && nH == 1 && pInv && nSlot >= 0 && nSlot < 16) {
        return INVENTORY_PlaceItemInGrid(pInv, pItem, nSlot, 0, 1, 0, 0);
    }
    return 0;
}
