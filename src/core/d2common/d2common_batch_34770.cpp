// 1.13c D2Common implementation batch (rows 0x34770, 0x2EA70, 0x1A610, 0x4DF60).
// Each body was matched against the cached 1.13c decompile and retail disassembly; where a
// vendored D2MOO 1.10f twin exists the body delegates to it, otherwise the stores are written here.
#include <cstdint>

#include "D2DataTbls.h"
#include "Path/Path.h"
#include "Units/Units.h"

// 1.13c D2Common+0x34770: last saved path step X (uint16, zero-extended) of a path that carries
// PATH_SAVE_STEPS_MASK; 0 when no steps were saved. The 1.10f twin asserts the same mask.
uint32_t D2Common_10253_PATH_GetSaveX(D2DynamicPathStrc* pDynamicPath)
{
    return (uint32_t)D2COMMON_10199_PathGetSaveX(pDynamicPath);
}

// 1.13c D2Common+0x2EA70: object units only (type 2); returns the object data's portal flags byte
// (pObjectData+5). Retail fatals 0xfe4 on a null unit and 0xfe5 on a non-object unit.
uint8_t D2Common_10258_UNITS_GetObjectPortalFlags(D2UnitStrc* pUnit)
{
    return UNITS_GetObjectPortalFlags(pUnit);
}

// 1.13c D2Common+0x1A610: monster units only (type 1); TRUE when MonStatsTxt[classId].dwMonStatsFlags
// carries MONSTATSFLAG_DEMON (bit 13). Retail tests byte 0x0d of the 0x1a8-byte row with bit 0x20.
BOOL D2Common_10255_MONSTER_IsDemon(D2UnitStrc* pUnit)
{
    if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER)
    {
        return FALSE;
    }

    const D2MonStatsTxt* pMonStatsTxtRecord = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
    if (!pMonStatsTxtRecord)
    {
        return FALSE;
    }

    return (pMonStatsTxtRecord->dwMonStatsFlags & MONSTATSFLAG_DEMON) != 0;
}

// 1.13c D2Common+0x4DF60: null unit -> fatal 0xa77 (modelled as a return); retail stores the value
// as a dword at unit+0x18, i.e. the act byte plus its three padding bytes.
void D2Common_10243_UNITS_SetUnitAct(D2UnitStrc* pUnit, uint32_t nValue)
{
    if (!pUnit)
    {
        return;
    }

    *reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(pUnit) + 0x18) = nValue;
}
