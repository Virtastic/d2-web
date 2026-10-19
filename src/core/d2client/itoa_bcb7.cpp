// 1.13c D2Client+0xBCB7: CRT integer-to-string wrapper, defined radix 2..36 domain.
#include <cstdint>
extern "C" char* d2client_itoa_bcb7(int32_t input, char* output, uint32_t radix) {
    const int32_t value = input;
    char* const dest = output;
    
    const bool negative = radix == 10 && value < 0;
    uint32_t magnitude = static_cast<uint32_t>(value);
    if (negative) magnitude = 0u - magnitude;

    char* p = dest;
    if (negative) *p++ = '-';
    char* const first = p;
    do {
        const uint32_t digit = magnitude % radix;
        magnitude /= radix;
        *p++ = static_cast<char>(digit < 10 ? '0' + digit : 'a' + digit - 10);
    } while (magnitude != 0);
    *p = '\0';
    for (char* left = first, *right = p - 1; left < right; ++left, --right) {
        const char c = *left;
        *left = *right;
        *right = c;
    }
    return output;
}
