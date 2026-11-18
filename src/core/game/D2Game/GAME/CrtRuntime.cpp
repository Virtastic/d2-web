// 1.13c D2Game's statically linked MSVC C runtime: the pieces the game bodies and probes reach by name.
// 1.10f's D2Game keeps no CRT of its own in the vendored tree, so these have no twin.
#include <cctype>
#include <cstdint>

#include "GAME/CrtRuntime.h"

char D2GAME_CRT_cDecimalPoint = '.';        // DAT_6fd1f994 (locale decimal point)
uint32_t D2GAME_CRT_dwStaticInitFlags = 0;  // DAT_6fd39fa4 (bit 0: the static initializer has run)

// 1.13c D2Game+0xF609C _forcdecpt: inserts the decimal point before the 'e' of a formatted float.
void __cdecl CRT_ForceDecimalPoint_F609C(char* pszNumber)
{
    bool bAtExponent = tolower((unsigned char)*pszNumber) == 'e';
    while (!bAtExponent)
    {
        ++pszNumber;
        bAtExponent = isdigit((unsigned char)*pszNumber) == 0;
    }

    char cCarry = *pszNumber;
    *pszNumber = D2GAME_CRT_cDecimalPoint;
    do
    {
        ++pszNumber;
        const char cNext = *pszNumber;
        *pszNumber = cCarry;
        cCarry = cNext;
    }
    while (*pszNumber != '\0');
}

// 1.13c D2Game+0xF78A0 Unwind@6fd178a0 (EAX = the stored dword afterwards)
uint32_t CRT_Unwind_F78A0()
{
    D2GAME_CRT_dwStaticInitFlags &= 0xFFFFFFFEu;
    return D2GAME_CRT_dwStaticInitFlags;
}
