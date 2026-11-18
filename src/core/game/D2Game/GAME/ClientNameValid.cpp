#include "GAME/Game113c.h"

// 1.13c D2Game+0x2A110
// EAX = name, EDI = bound (16 at the only call site, GAME_VerifyJoinGame). Walks at most nMaxLen characters: a NUL is accepted only after at least one letter;
// an apostrophe, hyphen or underscore is accepted once, never first and never last; A-Z and a-z count as letters, and reaching nMaxLen of them fails.
// Any other byte fails. (1.10f walked a fixed 16 and let a name end exactly at the bound.)
bool __fastcall GAME_IsClientNameValid(const char* szName, int32_t nMaxLen)
{
    if (nMaxLen <= 0)
    {
        return false;
    }

    bool bContainsSpecialCharacter = false;
    int32_t nCount = 0;
    for (;;)
    {
        const char c = *szName++;
        if (c == '\0')
        {
            return nCount != 0;
        }

        if (c == '\'' || c == '-' || c == '_')
        {
            if (bContainsSpecialCharacter || nCount == 0 || *szName == '\0')
            {
                return false;
            }
            bContainsSpecialCharacter = true;
        }
        else if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')))
        {
            return false;
        }

        if (++nCount >= nMaxLen)
        {
            return false;
        }
    }
}
