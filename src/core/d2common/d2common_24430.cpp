// 1.13c D2Common+0x24430 (ITEMS_CheckItemTypeId, retail body): the item's class row (items.txt wType[0], 0x230A0)
// tested in the equivalence bit table (sgptDataTables +0xC00 stride, +0xC04 table), then wType[1] (0x719A0 record +0x120).
// Differs from the 1.10f ITEMS_CheckItemTypeIdByItemId (LUT of wType[0]/wType[1] by type-equivalence): retail returns
// the masked word itself on the wType[1] branch, not a boolean. A class id past items.txt halts retail (_exit); here it returns 0.
#include <cstdint>
#include "D2DataTbls.h"
#include "DataTbls/ItemsTbls.h"
#include "Units/Units.h"
#include "D2Items.h"

static_assert(sizeof(D2ItemsTxt) == 0x1A8 && offsetof(D2ItemsTxt, wType) == 0x11E, "items.txt record (wType[0] at +0x11E)");

extern const uint32_t gdwBitMasks[32];

// Returns the masked table word (retail's value, not normalised to 0/1) or 1 on the wType[0] hit.
uint32_t D2Common_24430_CheckItemTypeId(const D2UnitStrc* pItem, int nItemType)
{
    if (!pItem) return 0;
    const int nRows = sgptDataTables->nItemTypesTxtRecordCount;
    if (nItemType < 0 || nItemType >= nRows) return 0;
    const uint32_t nClassId = pItem->dwClassId;
    if (nClassId >= (uint32_t)sgptDataTables->pItemDataTables.nItemsTxtRecordCount) return 0;   // retail: _exit(-1)
    const D2ItemsTxt* rec = &sgptDataTables->pItemDataTables.pItemsTxt[nClassId];
    const uint32_t* lut = sgptDataTables->pItemTypesEquivalenceLUTs;
    const int stride = sgptDataTables->nItemTypesIndex;
    const uint32_t bit = gdwBitMasks[nItemType & 31];
    const int nWordInRow = nItemType >> 5;

    if (lut[stride * (int)(int16_t)rec->wType[0] + nWordInRow] & bit) return 1;

    const int16_t nType2 = (int16_t)rec->wType[1];
    if (nType2 > 0 && nType2 < nRows) return lut[stride * (int)nType2 + nWordInRow] & bit;
    return 0;
}
