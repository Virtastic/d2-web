#include <cstdint>
#include <cstddef>

// 1.13c D2Common+0x026A0 _strcspn(str, control): MSVC CRT. Builds a 256-bit set of the
// bytes in control (terminator excluded), returns the count of leading str bytes not in it.
uint32_t d2common_strcspn_26a0(const char* str, const char* control)
{
    uint8_t set[32] = {};
    for (; *control; ++control) {
        uint8_t c = (uint8_t)*control;
        set[c >> 3] |= (uint8_t)(1u << (c & 7));
    }
    uint32_t n = 0;
    for (; *str; ++str, ++n) {
        uint8_t c = (uint8_t)*str;
        if ((set[c >> 3] >> (c & 7)) & 1) break;
    }
    return n;
}
