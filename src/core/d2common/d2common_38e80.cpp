// 1.13c D2Common+0x38E80: the stat-160 total of a unit (stdcall, one argument, ret 4). Same shape as
// 0x38E30 with the ItemStatCost count above 0xA0, row 160 (table + 160 * 0x144), packed id 0xA00000 (EDI).
#include <cstdint>
#include "D2DataTbls.h"
#include "D2StatList.h"
#include "Units/Units.h"

static_assert(sizeof(D2ItemStatCostTxt) == 0x144, "ItemStatCost row stride");

extern "C" int32_t D2Common_GetUnitStat160_38E80(D2UnitStrc* pUnit) // 1.13c D2Common+0x38E80
{
    if (!pUnit) return 0;
    D2StatListExStrc* pStatListEx = pUnit->pStatListEx; // +0x5C
    if (!pStatListEx) return 0;
    if (!sgptDataTables || sgptDataTables->nItemStatCostTxtRecordCount <= 0xA0 || !sgptDataTables->pItemStatCostTxt)
        return 0;
    return STATLIST_GetTotalStat_6FDB63E0(reinterpret_cast<D2StatListStrc*>(pStatListEx),
        D2SLayerStatIdStrc::MakeFromStatId(0xA0).nPackedValue, &sgptDataTables->pItemStatCostTxt[0xA0]);
}
