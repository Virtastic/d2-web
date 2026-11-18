#include <test_seam.h>
#include "GAME/Game113c.h"

#include <D2BitManip.h>
#include <D2DataTbls.h>

// 1.13c D2Game+0x2790
// EAX = unit, EDI = MonType index. Zero unless the unit is a monster whose MonStats row has a MonType in range and the index is also in range;
// then the MonType's bit in the index's row of the nest table.
uint32_t __fastcall MONSTER_IsUnitMonTypeNestedIn(D2UnitStrc* pUnit, int32_t nIndex)
{
    if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER)
    {
        return 0;
    }

    const int32_t nClassId = pUnit->dwClassId;
    if (nClassId < 0 || nClassId >= sgptDataTables->nMonStatsTxtRecordCount)
    {
        return 0;
    }

    const D2MonStatsTxt* pMonStatsTxtRecord = &sgptDataTables->pMonStatsTxt[nClassId];
    if (!pMonStatsTxtRecord)
    {
        return 0;
    }

    const int32_t nMonType = (int16_t)pMonStatsTxtRecord->wMonType;
    if (nMonType < 0 || nMonType >= sgptDataTables->nMonTypeTxtRecordCount || nIndex < 0 || nIndex >= sgptDataTables->nMonTypeTxtRecordCount)
    {
        return 0;
    }

    return gdwBitMasks[nMonType & 31] & sgptDataTables->pMonTypeNest[(nMonType >> 5) + nIndex * sgptDataTables->nMonTypeIndex];
}
