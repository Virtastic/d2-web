#include <test_seam.h>
#include "GAME/Game113c.h"

#include <D2DataTbls.h>

// 1.13c D2Game+0x19F0
// EAX = monster class id (the stack argument is ignored). The MonStats row's wMonProp selects the MonProp row; null for any id or row that is out of range.
D2MonPropTxt* __fastcall MONSTER_GetMonPropTxtRecordFromClass(int32_t nClassId)
{
    if (nClassId < 0 || nClassId >= sgptDataTables->nMonStatsTxtRecordCount)
    {
        return nullptr;
    }

    const D2MonStatsTxt* pMonStatsTxtRecord = &sgptDataTables->pMonStatsTxt[nClassId];
    if (!pMonStatsTxtRecord)
    {
        return nullptr;
    }

    const int16_t nMonProp = (int16_t)pMonStatsTxtRecord->wMonProp;
    if (nMonProp < 0 || nMonProp >= sgptDataTables->nMonPropTxtRecordCount)
    {
        return nullptr;
    }

    return &sgptDataTables->pMonPropTxt[nMonProp];
}
