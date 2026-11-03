// 1.13c D2Common+0x38E30: the stat-159 total of a unit (stdcall, one argument, ret 4). Same shape as
// 0x38DE0: unit+0x5C is the stat list, ItemStatCost count above 0x9F, row 159 (table + 159 * 0x144),
// packed id 0x9F0000 (EDI).
#include <cstdint>
#include "D2DataTbls.h"
#include "D2StatList.h"
#include "Units/Units.h"

static_assert(sizeof(D2ItemStatCostTxt) == 0x144, "ItemStatCost row stride");

extern "C" int32_t D2Common_38E30_GetUnitStat159(D2UnitStrc* pUnit) // 1.13c D2Common+0x38E30
{
    if (!pUnit) return 0;
    D2StatListExStrc* pStatListEx = pUnit->pStatListEx; // +0x5C
    if (!pStatListEx) return 0;
    if (!sgptDataTables || sgptDataTables->nItemStatCostTxtRecordCount <= 0x9F || !sgptDataTables->pItemStatCostTxt)
        return 0;
    return STATLIST_GetTotalStat_6FDB63E0(reinterpret_cast<D2StatListStrc*>(pStatListEx),
        D2SLayerStatIdStrc::MakeFromStatId(0x9F).nPackedValue, &sgptDataTables->pItemStatCostTxt[0x9F]);
}
