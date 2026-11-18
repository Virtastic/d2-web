#include <test_seam.h>
#include "GAME/Game113c.h"

#include <D2BitManip.h>

// 1.13c D2Game+0x1CD0
// ESI = bit buffer, EDI = value. Below 0x100: flag 0, then 8 bits; below 0x10000: flags 1, 0, then 16 bits; otherwise flags 1, 1, then 32 bits.
void __fastcall GAME_WriteVariableWidthBits(D2BitBufferStrc* pBuffer, uint32_t nValue)
{
    if (nValue < 0x100)
    {
        D2_CALL(BITMANIP_Write, pBuffer, 0, 1);
        D2_CALL(BITMANIP_Write, pBuffer, nValue, 8);
    }
    else if (nValue < 0x10000)
    {
        D2_CALL(BITMANIP_Write, pBuffer, 1, 1);
        D2_CALL(BITMANIP_Write, pBuffer, 0, 1);
        D2_CALL(BITMANIP_Write, pBuffer, nValue, 16);
    }
    else
    {
        D2_CALL(BITMANIP_Write, pBuffer, 1, 1);
        D2_CALL(BITMANIP_Write, pBuffer, 1, 1);
        D2_CALL(BITMANIP_Write, pBuffer, nValue, 32);
    }
}
