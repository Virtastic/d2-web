// Real flat exports for D2Win text width. The glyph getter is retail's own data pointer (DAT_6f901508).
#include <cstdint>
#include <cstring>
extern "C" {
uint8_t* (*d2win_glyph_lookup)(uint16_t);  // DAT_6f901508 (b72 declares it weak); retail passes the char in CX
// 1.13c D2Win+0x12080 (#10055)
int32_t d2win_Ordinal_10055(const int16_t* s, int32_t n) {
  int32_t w = 0;
  for (int32_t i = 0; *s != 0 && i < n; ++i, ++s)
    if (*s != 10) w += d2win_glyph_lookup((uint16_t)*s)[3];
  return w;
}
// 1.13c D2Win+0x12B20 (#10028): width of a whole UTF-16 string
int32_t d2win_Ordinal_10028(const uint16_t* s) {
  int32_t n = 0; while (s[n]) ++n;  // Unicode::strlen
  return d2win_Ordinal_10055((const int16_t*)s, n);
}
}
