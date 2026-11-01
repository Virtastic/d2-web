// 1.13c D2Common+0x1A610 (__fastcall-free, 1 stack arg, ret 4): monster-table row flag test.
// Monster units only (type 1, index in range, row pointer non-null); the flag is byte 0x0D of the 0x1A8-byte
// MonStatsTxt row ANDed with byte 0x14 of the gdwBitMasks table (the low byte of entry 5).
#include <cstdint>
#include "D2DataTbls.h"
#include "D2BitManip.h"
#include "Units/Units.h"

extern "C" BOOL D2Common_1A610_UnitTableRowFlag(D2UnitStrc* pUnit)
{
    if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER || pUnit->dwClassId < 0)
    {
        return FALSE;
    }
    if (pUnit->dwClassId >= DATATBLS_GetMonStatsTxtRecordCount())
    {
        return FALSE;
    }
    const uint8_t* pRow = reinterpret_cast<const uint8_t*>(DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId));
    if (!pRow)
    {
        return FALSE;
    }
    return (pRow[0x0D] & reinterpret_cast<const uint8_t*>(gdwBitMasks)[0x14]) != 0;
}
