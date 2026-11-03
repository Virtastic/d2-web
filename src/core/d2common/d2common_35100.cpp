// 1.13c D2Common+0x35100: path type of a monster unit from its MonStats record (plain ret, unit in EAX).
// Records are read at the retail stride 0x1A8 from sgptDataTables+0xA78; the count is at +0xA80.
// Flying (record byte +0xD, bit 0x40) answers 0x1804; open doors (record byte +0xC, bit 0x08) answers
// 0x3401; anything else answers 0x3C01. Retail reads address 0x0D for a unit without a valid record
// (an access fault), so the port aborts the same way.
#include <cstdint>
#include <cstdlib>
#include "D2DataTbls.h"
#include "Units/Units.h"

extern "C" uint32_t D2Common_35100_MonsterPathType(D2UnitStrc* pUnit) // 1.13c D2Common+0x35100
{
    if (!pUnit || !sgptDataTables || !sgptDataTables->pMonStatsTxt)
        std::abort();
    const int32_t nClass = pUnit->dwClassId;
    if (nClass < 0 || nClass >= sgptDataTables->nMonStatsTxtRecordCount)
        std::abort();
    const uint8_t* pRec = reinterpret_cast<const uint8_t*>(sgptDataTables->pMonStatsTxt) + static_cast<size_t>(nClass) * 0x1A8;
    if (pRec[0xD] & 0x40)
        return 0x1804;
    return (pRec[0xC] & 0x08) ? 0x3401u : 0x3C01u;
}
