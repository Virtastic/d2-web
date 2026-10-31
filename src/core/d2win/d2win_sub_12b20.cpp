// 1.13c D2Win+0x12B20 (size 19): width of a whole UTF-16 string. fastcall(ecx = str): D2Lang Unicode::strlen, then a tail
// jump to 0x12080 with ecx = str, edx = length (its result is the return value).
#include <cstdint>
// textWidth: D2Win+0x12080 (per-glyph width sum over the current font, skips '\n').
// 1.13c D2Win+0x12B20
extern "C" int32_t d2win_sub_12b20(int32_t (*textWidth)(const uint16_t* s, int32_t n), const uint16_t* s) {
  int32_t n = 0; while (s[n]) ++n;  // Unicode::strlen
  return textWidth(s, n);
}
