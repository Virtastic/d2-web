// 1.13c D2Common batch 14 (RE-DONE alpha lane): items.txt row accessors and item predicates.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>
#include "D2DataTbls.h"
#include "DataTbls/ItemsTbls.h"
#include "D2Items.h"

// Implemented in d2common_batch_1a330.cpp / itemtype_getters.cpp / sub_227c0.cpp.
uint32_t D2Common_230A0_ItemTypeFromItemId(uint32_t nItemId);
uint32_t D2Common_227C0_ItemBitField1Bit0(uint32_t nItemId);
uint32_t D2Common_22F80_ItemTypeThrowable(int32_t nItemType);

// 1.13c D2Common+0x22BC0 (#10176): items.txt nQuest (+0x12A) of an item unit.
// pUnit must be non-null and a UNIT_ITEM (+0x00 == 4); its class id (+0x04) indexes items.txt.
// A null unit (fatal 0x9DB), wrong type (0x9DC) or missing row (0x9DF) is fatal in retail;
// we return 0 on each, the same as DATATBLS_GetItemsTxtRecord's out-of-range answer.
EMSCRIPTEN_KEEPALIVE uint8_t D2Common_22BC0_ItemGetQuest(const int32_t* pUnit)
{
    static_assert(offsetof(D2ItemsTxt, nQuest) == 0x12A, "items.txt nQuest");
    if (pUnit == nullptr || pUnit[0] != 4)
        return 0;
    const D2ItemsTxt* pRow = DATATBLS_GetItemsTxtRecord(pUnit[1]);
    return pRow != nullptr ? pRow->nQuest : 0;
}

static_assert(offsetof(D2ItemsTxt, dwCode) == 0x80, "items.txt dwCode");

// Inventory +0x0C (pFirstItem) holds a UNIT_ITEM; the retail routine reads the unit from EAX.
static bool Item_FirstInventoryItemIsItem(const int32_t* pUnit)
{
    const uint8_t* pInventory = pUnit != nullptr
        ? *reinterpret_cast<const uint8_t* const*>(reinterpret_cast<const uint8_t*>(pUnit) + 0x60)
        : nullptr;
    if (pInventory == nullptr || *reinterpret_cast<const uint32_t*>(pInventory) != 0x1020304)
        return false;
    const uint8_t* pFirst = *reinterpret_cast<const uint8_t* const*>(pInventory + 0x0C);
    return pFirst != nullptr && *reinterpret_cast<const uint32_t*>(pFirst) == 4;
}

// 1.13c D2Common+0x250B0 (#10185): item eligibility predicate.
// Null unit is fatal 0x67E in retail (we answer 0). Reject when the class id maps to item type 4
// or when the item data's dwItemFlags (+0x18) carries 0x1000. Otherwise, only for items whose
// items.txt dwBitField1 (+0xDC) has bit 0: reject a throwable item unless unit+0xC8 bit 25 is set;
// reject a quest item whose dwCode (+0x80) is not ' leg'; accept only when the inventory's first
// item is not an item, dwItemFlags bit 0x800 is clear, and the quality is none of 4..9.
// The three callees still UNMAPPED as rows (0x241A0, 0x23290, 0x24C30) are inlined here.
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_250B0_ItemPredicate(const int32_t* pUnit)
{
    if (pUnit == nullptr)
        return 0;
    if (pUnit[0] == 4 && D2Common_230A0_ItemTypeFromItemId(static_cast<uint32_t>(pUnit[1])) == 4)
        return 0;
    const uint8_t* pItemData = pUnit[5] != 0
        ? *reinterpret_cast<const uint8_t* const*>(reinterpret_cast<const uint8_t*>(pUnit) + 0x14)
        : nullptr;
    const uint32_t dwItemFlags = (pUnit[0] == 4 && pItemData != nullptr) ? *reinterpret_cast<const uint32_t*>(pItemData + 0x18) : 0;
    if (pUnit[0] == 4 && pItemData != nullptr && (dwItemFlags & 0x1000) != 0)
        return 0;
    const uint32_t nClassId = static_cast<uint32_t>(pUnit[1]);
    if (D2Common_227C0_ItemBitField1Bit0(nClassId) == 0)
        return 0;
    const uint32_t dwUnitFlags = *reinterpret_cast<const uint32_t*>(reinterpret_cast<const uint8_t*>(pUnit) + 0xC8);
    if ((dwUnitFlags >> 25 & 1) == 0
        && D2Common_22F80_ItemTypeThrowable(static_cast<int32_t>(D2Common_230A0_ItemTypeFromItemId(nClassId))) != 0)
        return 0;
    const D2ItemsTxt* pRow = DATATBLS_GetItemsTxtRecord(static_cast<int32_t>(nClassId));
    if (pRow != nullptr && pRow->nQuest != 0 && pRow->dwCode != 0x2067656C)  // ' leg'
        return 0;
    const int32_t nQuality = ITEMS_GetItemQuality(const_cast<D2UnitStrc*>(reinterpret_cast<const D2UnitStrc*>(pUnit)));
    if (!Item_FirstInventoryItemIsItem(pUnit) && (dwItemFlags & 0x800) == 0
        && nQuality != 4 && nQuality != 5 && nQuality != 6 && nQuality != 7 && nQuality != 8 && nQuality != 9)
        return 1;
    return 0;
}
