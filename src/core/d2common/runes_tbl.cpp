// src/core/d2common/runes_tbl.cpp -- the 1.13c D2Common+0x70FF0 body:
// stdcall(index, record) writes one 0x120-byte rune record into the rune table, growing the table to index+1
// records through the Fog realloc-pool first when the index is at or beyond the current count.
#include "runes_tbl.h"
#include <cstring>

void D2Common_70FF0_AddOrChangeRunesTxtRecord(const RunesTblEnv& e, int32_t nRecordId, const uint8_t* rec)
{
    constexpr uint32_t kStride = 0x120;   // sizeof(D2RunesTxt) in the image

    if (*e.pCount <= (uint32_t)nRecordId)
    {
        uint32_t nSize = kStride * (uint32_t)(nRecordId + 1);
        uint8_t* pOld = *e.pArray;
        if (e.mode) { pOld -= 4; nSize += 4; }

        uint8_t* p = e.realloc(pOld, nSize, e.ctx);
        if (e.mode) { *(uint32_t*)p = (uint32_t)(nRecordId + 1); p += 4; }
        *e.pArray = p;

        const uint32_t nGap = (uint32_t)nRecordId - *e.pCount;
        if (nGap) memset(p + *e.pCount * kStride, 0, nGap * kStride);
        *e.pCount = (uint32_t)(nRecordId + 1);
    }

    memcpy(*e.pArray + (uint32_t)nRecordId * kStride, rec, kStride);
}
