// 1.13c D2Common+0x6B50 strlen: pointer traversal is native, result wraps as retail size_t. the 1.10f source has no twin (host CRT).
#include <cstdint>

// 1.13c D2Common+0x6B50
extern "C" uint32_t d2common_strlen_6b50(const char* text)
{
    uint32_t length = 0;
    while (*text++ != '\0') ++length;
    return length;
}
