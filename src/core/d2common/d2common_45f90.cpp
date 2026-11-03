// 1.13c D2Common+0x45F90: apply the property lists of a set item's set for the owner's current
// set-item mask. Registers: EBX = the set item; stack: unit (the owner), state (0 = do nothing).
#include <algorithm>
#include <cstdint>
#include "D2Items.h"
#include "D2DataTbls.h"
#include "D2ItemMods.h"
#include "Units/Units.h"

extern int gnSetMaskToBonusMappingTable[64];   // 0x6FDE3200 (same table as 0x2C7C0)

// 1.13c D2Common+0x45F90
void D2Common_45F90_SetBonuses(D2UnitStrc* unit, D2UnitStrc* item, int state)
{
    if (!state || !item || item->dwUnitType != UNIT_ITEM || ITEMS_GetItemQuality(item) != ITEMQUAL_SET) return;
    const int index = ITEMS_GetFileIndex(item);
    if (index < 0 || index >= sgptDataTables->nSetItemsTxtRecordCount) return;
    const auto& setItem = sgptDataTables->pSetItemsTxt[index];
    if (setItem.nSetId < 0 || setItem.nSetId >= sgptDataTables->nSetsTxtRecordCount) return;
    auto& set = sgptDataTables->pSetsTxt[setItem.nSetId];
    const uint32_t mask = ITEMS_GetSetItemsMask(unit, item, 1);
    const int count = mask < 64 ? gnSetMaskToBonusMappingTable[mask] : 0;
    // the partial list is bounded by the clamped count, the full-set test uses the raw count
    const int partial = 2 * std::min(count, set.nSetItems - 1) - 2;
    for (int i = 0; i < partial; ++i)
        if (set.pBoni2[i].nProperty >= 0) sub_6FD95810(4, unit, item, nullptr, i, 0, 0, &set.pBoni2[i], state, 0, nullptr);
    if (count >= set.nSetItems)
        for (int i = 0; i < 8 && set.pFBoni[i].nProperty >= 0; ++i)
            sub_6FD95810(4, unit, item, nullptr, i, 0, 0, &set.pFBoni[i], state, 0, nullptr);
}
