// 1.13c D2Win+0x12080 (GetUnicodeTextNDrawWidth, ord 10055): sum glyph widths of the first n wchar_t
// units of a 16-bit string, skipping '\n'; stops at NUL. Stateful (current-font glyph getter
// DAT_6f901508 returns a glyph record whose byte +3 is the advance); NAMED only, no vector.
#include <cstdint>

struct D2WinGlyphEnv {
    const uint8_t* (*glyph)();  // DAT_6f901508
};

int32_t d2win_GetUnicodeTextNDrawWidth(const int16_t* s, int32_t n, const D2WinGlyphEnv& env) {
    int32_t w = 0;
    for (int32_t i = 0; *s != 0 && i < n; ++i, ++s)
        if (*s != 10) w += env.glyph()[3];
    return w;
}
