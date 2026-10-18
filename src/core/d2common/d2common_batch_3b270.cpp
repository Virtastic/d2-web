// 1.13c D2Common batch: the two rows whose bodies differ from every vendored 1.10f function.
// Retail guest pointers are 4 bytes; these functions take native pointers here (m64), so the
// local AnimData lookup below walks the same table through the vendored structures.
#include <cstdint>
#include <cstring>
#include "D2DataTbls.h"
#include "D2Composit.h"
#include "Units/Units.h"
// 1.13c D2Common+0x3B270: accessor, returns the unit's fixed-point frame counter (UnitAny+0x48).
// Retail fatals on a null unit; we return 0.
uint32_t D2Common_10047_GetUnitFrameCount(D2UnitStrc* param_1)
{
    return param_1 ? (uint32_t)param_1->dwFrameCountPrecise : 0;
}
// 1.13c D2Common+0x41FD0: builds the unit's composit/COF path (0x44290) and stores nAnimSpeed into
// the dwAnimSpeed field of the matching AnimData.d2 record; no record -> no write.
// 1.10f DATATBLS_UnitAnimInfoDebugSet does the same but halts afterwards; 1.13c returns.
static D2AnimDataRecordStrc* Batch3_FindAnimDataRecord(char* szPath)
{
    for (char* p = szPath; *p; ++p)
    {
        if (*p >= 'a' && *p <= 'z') *p -= ' ';          // uppercase in place
    }
    uint8_t nHash = 0;
    for (const char* p = szPath; *p; ++p)
    {
        nHash += (uint8_t)*p;                          // byte-sum hash modulo 256
    }
    D2AnimDataBucketStrc* pBucket = DATATBLS_GetAnimData()->pHashTableBucket[nHash];
    for (int i = 0; i < pBucket->nbEntries; ++i)
    {
        D2AnimDataRecordStrc* pRecord = &pBucket->aEntries[i];
        // Names are NUL-padded to 8 bytes; retail compares exactly 8 bytes and fatals above 8.
        if (memcmp(szPath, pRecord->szAnimDataName, 8) == 0) return pRecord;
    }
    return nullptr;
}
// 1.13c D2Common+0x41FD0: set the AnimData speed for the unit's current composit.
void D2Common_10063_UnitAnimInfoSet(D2UnitStrc* pUnit, int nAnimSpeed)
{
    char szPath[8] = {};
    int nWeaponClassCode = 0;
    D2Common_10885_COMPOSIT_unk(pUnit, szPath, &nWeaponClassCode, FALSE, 1, pUnit->pInventory, -1);
    if (D2AnimDataRecordStrc* pRecord = Batch3_FindAnimDataRecord(szPath)) pRecord->dwAnimSpeed = nAnimSpeed;
}
