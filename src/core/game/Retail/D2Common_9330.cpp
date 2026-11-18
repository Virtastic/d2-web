// CRT strrchr, moved from the stand-alone d2common batch into the game tree.
#include <cstdint>

// 1.13c D2Common+0x09330
char* d2common_strrchr_9330(char* text, int character)
{
    char* last = nullptr;
    const char needle = static_cast<char>(character);
    do
    {
        if (*text == needle)
            last = text;
    } while (*text++ != '\0');
    return last;
}
