#include <cstdint>

// 1.13c D2Common+0x06970 strcat(dst, src): MSVC CRT. Finds dst's terminator, copies src
// including its NUL there, returns dst. The original works a word at a time (alignment
// head, zero-byte bit trick); the byte result is identical for non-overlapping buffers.
char* d2common_strcat_6970(char* dst, const char* src)
{
    char* p = dst;
    while (*p) ++p;
    while ((*p++ = *src++) != 0) {}
    return dst;
}
