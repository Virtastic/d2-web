#include <cstddef>
#include <cstdint>
#include <initializer_list>

// CRT string routines; retail's word-at-a-time tricks are an implementation detail, semantics are libc's.

// 1.13c D2sound+0x40C0
static char* D2sound_b57a_strcpy_40C0(char* dst, const char* src)
{
    char* d = dst;
    while ((*d++ = *src++) != 0) {}
    return dst;
}

// 1.13c D2sound+0x40D0
static char* D2sound_b57a_strcat_40D0(char* dst, const char* src)
{
    char* d = dst;
    while (*d) ++d;
    while ((*d++ = *src++) != 0) {}
    return dst;
}

// 1.13c D2sound+0x41C0
static size_t D2sound_b57a_strlen_41C0(const char* s)
{
    const char* p = s;
    while (*p) ++p;
    return (size_t)(p - s);
}

// 1.13c D2sound+0x4690
static char* D2sound_b57a_strncpy_4690(char* dst, const char* src, size_t n)
{
    size_t i = 0;
    for (; i < n && src[i]; ++i) dst[i] = src[i];
    for (; i < n; ++i) dst[i] = 0;
    return dst;
}
