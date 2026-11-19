#include "GAME/Game113c.h"

#include <cstddef>
#include <cstdint>

// 1.13c D2Game+0x3520 (_strncmp): retail first counts s1's bytes through its terminator (capped at maxCount), then compares; the first
// differing byte gives 1 or -1.
int __cdecl D2GAME_CRT_strncmp_6FC23520(const char* s1, const char* s2, size_t maxCount)
{
    if (maxCount == 0) return 0;
    size_t limit = 0;
    while (limit < maxCount && s1[limit] != '\0') ++limit;
    if (limit < maxCount) ++limit; // include the compared terminator
    for (size_t i = 0; i < limit; ++i)
    {
        const unsigned char a = (unsigned char)s1[i], b = (unsigned char)s2[i];
        if (a != b) return b <= a ? 1 : -1;
    }
    return 0;
}

// 1.13c D2Game+0x2E50 (_strncpy): copies up to count bytes, zero-filling the remainder after the terminator.
char* __cdecl D2GAME_CRT_strncpy_6FC22E50(char* dest, const char* source, size_t count)
{
    if (count == 0) return dest;
    char* d = dest;
    for (;;)
    {
        const char c = *source++;
        *d++ = c;
        if (--count == 0) return dest;
        if (c == '\0') break;
    }
    while (count-- > 0) *d++ = '\0';
    return dest;
}
