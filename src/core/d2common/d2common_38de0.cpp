// 1.13c D2Common+0x38DE0: the stat-31 total of a unit (stdcall, one argument, ret 4).
// The unit's stat list is unit+0x5C. Retail requires the ItemStatCost count above 0x1F and a
// non-null table row (table + 31 * 0x144); the total is STATLIST_GetTotalStat with the packed id
// 0x1F0000 (EDI in the 1.13c disassembly), which the 0x38A80 getter performs.
#include <cstdint>
#include "D2DataTbls.h"
#include "D2StatList.h"
#include "Units/Units.h"

static_assert(sizeof(D2ItemStatCostTxt) == 0x144, "ItemStatCost row stride");

extern "C" int32_t D2Common_38DE0_GetDefenseStat(D2UnitStrc* pUnit) // 1.13c D2Common+0x38DE0
{
    if (!pUnit) return 0;
    D2StatListExStrc* pStatListEx = pUnit->pStatListEx; // +0x5C
    if (!pStatListEx) return 0;
    if (!sgptDataTables || sgptDataTables->nItemStatCostTxtRecordCount <= 0x1F || !sgptDataTables->pItemStatCostTxt)
        return 0;
    return STATLIST_GetTotalStat_6FDB63E0(reinterpret_cast<D2StatListStrc*>(pStatListEx),
        D2SLayerStatIdStrc::MakeFromStatId(0x1F).nPackedValue, &sgptDataTables->pItemStatCostTxt[0x1F]);
}
