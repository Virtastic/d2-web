// 1.13c D2Common+0x240d0: whether an item may carry a personalised name (stdcall: item).
// Retail's null-item path calls _exit(-1); this port returns false for it and no fixture reaches it.
#include <cstdint>
#include "D2Items.h"
#include "DataTbls/ItemsTbls.h"
#include "Units/Units.h"
#include "Units/Item.h"

uint32_t D2Common_230A0_ItemTypeFromItemId(uint32_t dwItemId);
uint8_t D2Common_222B0(const D2UnitStrc* pItem);

bool D2Common_240D0(D2UnitStrc* pItem) {
    if (!pItem) {
        return false;
    }
    if (pItem->dwUnitType == 4 && D2Common_230A0_ItemTypeFromItemId((uint32_t)pItem->dwClassId) == 4) {
        return false;
    }
    // Item data flag 0x1000 excludes the item; an item with no item data, or a non-item unit, passes this test.
    const bool bPassFlag = pItem->dwUnitType != 4 || !pItem->pItemData || (pItem->pItemData->dwItemFlags & 0x1000) == 0;
    if (!bPassFlag) {
        return false;
    }
    const uint32_t nType = ITEMS_GetItemType(pItem);
    if (nType == 7 || nType == 5 || nType == 6) {
        return false;
    }
    if (pItem->dwUnitType == 4 && pItem->pItemData && (pItem->pItemData->dwItemFlags & 0x100) != 0) {
        return false;
    }
    if (pItem->dwUnitType == 4 && pItem->pItemData && (pItem->pItemData->dwItemFlags & 0x1000000) != 0) {
        return false;
    }
    if (D2Common_222B0(pItem) != 0) {
        return false;
    }
    const D2ItemsTxt* pRec = DATATBLS_GetItemsTxtRecord((int)pItem->dwClassId);
    return pRec && pRec->nNameable != 0;
}
