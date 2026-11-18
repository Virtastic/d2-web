#include <test_seam.h>
#include "GAME/Game113c.h"

#include <cstdint>
#include <cstring>

// The CRT's own state is not modelled: this runtime only has the startup locale, whose wlocale is null (the C locale), so __stricmp takes the
// ___ascii_stricmp (D2Game+0x3940) path retail takes for it.
int d2_fn_d2game_3940(const char* str1, const char* str2);

// 1.13c D2Game+0x2813 (__stricmp)
// Retail refreshes the thread's ptlocinfo (0x3B1F) when it is not the startup locale; with a null wlocale (locinfo+0x14) it forwards to 0x3940,
// else it folds each byte with ___tolower_mt (0x3853) and returns the first difference. Only the first path exists here.
int __cdecl D2GAME_CRT_stricmp_6FC22813(const char* pString1, const char* pString2)
{
    return d2_fn_d2game_3940(pString1, pString2);
}
