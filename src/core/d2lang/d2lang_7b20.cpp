// Prior art used: MSVC CRT _strrchr semantics (the last occurrence; the terminator counts).
#include <cstdint>

// 1.13c D2Lang+0x07B20: _strrchr(char* s, int ch). Last occurrence of (char)ch in the NUL-terminated string,
// the terminator included (ch == 0 finds it); NULL when absent.
char* d2lang_7b20(const char* s, int32_t ch)
{
    char c = (char)ch;
    const char* last = nullptr;
    for (;; ++s) {
        if (*s == c)
            last = s;
        if (*s == '\0')
            break;
    }
    return (char*)last;
}
