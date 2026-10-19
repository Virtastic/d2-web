// MSVC's C-locale byte comparison: fold ASCII A-Z to a-z, then return the
// sign of the first differing folded bytes. The preceding raw-byte equality
// check lets equal non-ASCII bytes pass without locale conversion.
#include <cstdint>

// 1.13c D2Game+0x3940 (___ascii_stricmp)
int d2_fn_d2game_3940(const char* str1, const char* str2)
{
    uint8_t last2 = 0xff;
    for (;;) {
        if (last2 == 0) return 0;
        last2 = static_cast<uint8_t>(*str2++);
        const uint8_t first = static_cast<uint8_t>(*str1++);
        if (first == last2) continue;
        const uint8_t folded1 = first >= 'A' && first <= 'Z' ? uint8_t(first + 32) : first;
        const uint8_t folded2 = last2 >= 'A' && last2 <= 'Z' ? uint8_t(last2 + 32) : last2;
        if (folded1 != folded2) return folded1 < folded2 ? -1 : 1;
    }
}
