// 1.13c D2Common+0x24710: spell icon of an item unit. A type-4 unit with neither stat-bit check (type ids 0x16 and
// 0x12 via 0x24430, the vendored ITEMS_CheckItemTypeId) yields the items.txt nSpellIcon (byte +0x134) of its class;
// otherwise the book id (item data +0x3E) selects the books.txt row and yields its nSpellIcon (byte +0x02), or
// 0xFFFFFFFF when there is no such row. Null unit is fatal 0xDAD, non-item fatal 0xDAE, missing items row 0xDB9.
#include <cstdint>
#include "D2Items.h"
#include "DataTbls/ItemsTbls.h"
#include "Units/Units.h"

extern "C" {
__attribute__((weak)) uint32_t Ordinal_10265(uint32_t);
__attribute__((weak)) void Ordinal_10024(const void*, uint32_t);
__attribute__((weak)) char DAT_6fdda728[1];
}

// retail fatal: Ordinal_10024(&DAT_6fdda728, Ordinal_10265(code)); _exit(-1). Seams are weak; skip if unlinked.
[[noreturn]] static void d2_fatal(uint32_t code)
{
    if (Ordinal_10265 && Ordinal_10024)
        Ordinal_10024(DAT_6fdda728, Ordinal_10265(code));
    __builtin_trap();
}

extern "C"
uint32_t D2Common_24710_GetSpellIcon(D2UnitStrc* pItem)
{
    if (!pItem)
        d2_fatal(0xDAD);
    if (pItem->dwUnitType != UNIT_ITEM)
        d2_fatal(0xDAE);

    if (!ITEMS_CheckItemTypeId(pItem, 0x16) && !ITEMS_CheckItemTypeId(pItem, 0x12))
    {
        const D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(static_cast<int32_t>(pItem->dwClassId));
        if (!pItemsTxtRecord)
            d2_fatal(0xDB9);
        return pItemsTxtRecord->nSpellIcon;
    }

    const uint32_t nBookId = pItem->pItemData ? pItem->pItemData->wMagicSuffix[0] : 0;
    const D2BooksTxt* pBooksTxtRecord = DATATBLS_GetBooksTxtRecord(static_cast<int32_t>(nBookId));
    if (!pBooksTxtRecord)
        return 0xFFFFFFFFu;
    return pBooksTxtRecord->nSpellIcon;
}
