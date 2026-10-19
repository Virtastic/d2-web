#include <cstdint>

// 1.13c D2Common+0x026F0 -- the embedded VS CRT _strncpy.
extern "C" char* D2Common_Strncpy_26f0(char* dest, const char* src, uint32_t count) {
    char* const result = dest;
    while (count != 0) {
        const char value = *src++;
        *dest++ = value;
        --count;
        if (value == '\0') {
            while (count != 0) {
                *dest++ = '\0';
                --count;
            }
            break;
        }
    }
    return result;
}
