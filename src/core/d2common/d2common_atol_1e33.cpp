// _atol: skip isspace, optional sign, accumulate decimal digits with 32-bit wrap.
// ponytail: C-locale isspace (9..13, 32); retail consults the per-thread locale ctype table.
#include <cstdint>

// 1.13c D2Common+0x01E33
extern "C"
int32_t D2Common_1E33_Atol(const char* s)
{
    while (*s == ' ' || (*s >= 9 && *s <= 13)) ++s;
    uint32_t c = (uint8_t)*s++;
    const uint32_t sign = c;
    if (c == '-' || c == '+') c = (uint8_t)*s++;
    uint32_t n = 0;
    while (c >= '0' && c <= '9') { n = n * 10 + (c - '0'); c = (uint8_t)*s++; }
    return (int32_t)(sign == '-' ? 0u - n : n);
}
