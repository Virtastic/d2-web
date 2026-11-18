#include "GAME/Game113c.h"

// 1.13c D2Game+0x2E10 (__strrev)
// Reverses the string in place up to its NUL (which stays where it is) and returns it; an empty string is returned untouched.
char* __cdecl D2GAME_CRT_strrev_6FC22E10(char* szString)
{
    char* pEnd = szString;
    while (*pEnd != '\0')
    {
        ++pEnd;
    }

    if (pEnd == szString)
    {
        return szString;
    }

    for (char* pLo = szString, *pHi = pEnd - 1; pLo < pHi; ++pLo, --pHi)
    {
        const char c = *pLo;
        *pLo = *pHi;
        *pHi = c;
    }
    return szString;
}
