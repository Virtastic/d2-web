#include "GAME/Game113c.h"

#include <cstddef>
#include <cstdint>

// 1.13c D2Game+0x2AD0 (_memmove)
// Overlap-safe byte copy: the same pointer or a zero size returns the destination untouched; a destination below the source copies forward,
// otherwise backwards. Result is the destination.
void* __cdecl D2GAME_CRT_memmove_6FC22AD0(void* pDest, const void* pSrc, size_t nSize)
{
    uint8_t* pD = (uint8_t*)pDest;
    const uint8_t* pS = (const uint8_t*)pSrc;
    if (pD == pS || nSize == 0)
    {
        return pDest;
    }

    if ((uintptr_t)pD < (uintptr_t)pS)
    {
        for (size_t i = 0; i < nSize; ++i)
        {
            pD[i] = pS[i];
        }
    }
    else
    {
        for (size_t i = nSize; i != 0; --i)
        {
            pD[i - 1] = pS[i - 1];
        }
    }
    return pDest;
}
