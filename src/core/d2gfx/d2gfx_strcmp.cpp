#include <cstdint>

// Prior art: D2MOO D2Gfx has no corresponding CRT leaf; adjacent D2Gfx _strlen is
// byte-equivalent to its retail scan. The 1.13c Visual Studio 2003 _strcmp compares
// unsigned bytes and normalizes every unequal result to -1 or +1.
extern "C" {
// 1.13c D2Gfx+0x6840
int32_t d2gfx_strcmp(const char* lhs, const char* rhs) {
    const auto* a = reinterpret_cast<const uint8_t*>(lhs);
    const auto* b = reinterpret_cast<const uint8_t*>(rhs);
    for (;;) {
        const uint8_t x = *a++;
        const uint8_t y = *b++;
        if (x != y) return x < y ? -1 : 1;
        if (x == 0) return 0;
    }
}
}
