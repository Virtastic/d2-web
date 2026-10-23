// 1.13c D2Common implementation batch (rows 0x18E90, 0x1E630 and later additions).
// Stage A: each body is a transliteration of the cached 1.13c decompile. Fatal exits are modelled as
// returns (our engine does not abort) and pointer-sized loads follow the wasm64 port's 8-byte pointers.
#include <cstdint>
#include <cstddef>

#include "D2DataTbls.h"
#include "D2Inventory.h"

#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif

// D2AllocNewStats state: record bound and the two allocation arrays whose records are 0x34 bytes.
static uint32_t s_nStatsAllocCount;
static uintptr_t s_pStatsAllocArrays[2];


// 1.13c D2Common+0x18E90
extern "C"
EMSCRIPTEN_KEEPALIVE uintptr_t D2AllocNewStats(uint32_t nIndex, int32_t nArray)
{
    if (s_nStatsAllocCount <= nIndex)
    {
        return 0;
    }
    if (nArray < 0 || nArray > 1)
    {
        return 0;
    }
    uintptr_t pRecord = s_pStatsAllocArrays[nArray] + (uintptr_t)nIndex * 0x34u;
    if (pRecord != 0)
    {
        return pRecord;
    }
    // retail fatals 0x126 for array 0 and 0x129 for array 1; modelled as a return
    return 0;
}

// 1.13c D2Common+0x1E630
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2UpdateDisabledStat(uintptr_t pHeader, uintptr_t pStat)
{
    if (pHeader != 0 && *(const uint32_t*)pHeader == 0x01020304u &&
        pStat != 0 && *(const uint32_t*)pStat == 4u)
    {
        // 0x1050 (type 4 branch): dword at [unit+0x2c]+0xc
        const uint32_t* u = (const uint32_t*)pStat;
        uint32_t nValue = *(const uint32_t*)((uintptr_t)u[0xb] + 0xc);
        // retail keeps the value only when it is <= 10
        return nValue & (uint32_t)((10 < (int32_t)nValue) - 1);
    }
    return 0;
}

// 1.13c D2Common+0x3C020
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_3C020_TakeUnitDataValue(uintptr_t pUnit)
{
    if (pUnit == 0)
    {
        return 0; // retail fatals 699; modelled as a return
    }
    uint8_t* pUnitData = *(uint8_t**)(pUnit + 0x10);
    uint32_t nFlags = *(uint32_t*)(pUnitData + 0x28);
    if ((nFlags & 0x40000u) == 0)
    {
        if ((nFlags & 0x4000000u) != 0)
        {
            return 0;
        }
        *(uint32_t*)(pUnitData + 0x28) = nFlags | 0x4000000u;
    }
    return *(uint32_t*)(pUnitData + 0x5c);
}

// 1.13c D2Common+0x34720
extern "C"
EMSCRIPTEN_KEEPALIVE uint16_t d2_fn_d2common_34720(uintptr_t pPath)
{
    if ((*(const uint32_t*)(pPath + 0x34) & 0x20000u) == 0)
    {
        return 0; // retail fatals 0x911; modelled as a return
    }
    int32_t nIndex = *(const int32_t*)(pPath + 0x1d4);
    if (nIndex != 0)
    {
        return *(const uint16_t*)(pPath + 0x1d6 + (intptr_t)nIndex * 4);
    }
    return 0;
}

// 1.13c D2Common+0x22C80
extern "C"
EMSCRIPTEN_KEEPALIVE uint8_t D2Common_22C80_GetItemTxtByte11D(uintptr_t pItem)
{
    if (pItem != 0 && *(const uint32_t*)pItem == 4u)
    {
        const uint8_t* pItemsTxt = (const uint8_t*)DATATBLS_GetItemsTxtRecord(*(const int32_t*)(pItem + 4));
        if (pItemsTxt == nullptr)
        {
            return 0; // retail fatals 0x9c5; modelled as a return
        }
        return pItemsTxt[0x11d];
    }
    return 0;
}

// 1.13c D2Common+0x2F720
extern "C"
EMSCRIPTEN_KEEPALIVE void D2Common10472(uintptr_t pPlayer, uintptr_t pOutValue, uintptr_t pOutFlags)
{
    if (pPlayer != 0 && *(const uint32_t*)pPlayer == 0u)
    {
        uint8_t* pPlayerData = *(uint8_t**)(pPlayer + 0x14);
        if (pPlayerData == nullptr)
        {
            return; // retail fatals 0x145a; modelled as a return
        }
        *(uint32_t*)pOutValue = *(const uint32_t*)(pPlayerData + 0x80);
        *(uint32_t*)pOutFlags = *(const uint32_t*)(pPlayerData + 0x88);
        *(uint32_t*)(pPlayerData + 0x70) = 0;
        *(uint32_t*)(pPlayerData + 0x78) = 0xffffffffu;
        *(uint32_t*)(pPlayerData + 0x90) = 0xffffffffu;
        *(uint32_t*)(pPlayerData + 0x94) = 0;
        return;
    }
    *(uint32_t*)pOutValue = 0;
    *(uint32_t*)pOutFlags = 0xffffffffu;
}
