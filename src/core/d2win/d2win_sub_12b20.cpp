// 1.13c D2Win+0x12B20 (size 19): width of a whole UTF-16 string = 0x12080(str, strlen(str)).
#include <cstdint>
struct D2Win12B20Env {
  int32_t (*textWidth)(const uint16_t* s, int32_t n);  // D2Win+0x12080 (per-glyph width sum, skips '\n')
};
// 1.13c D2Win+0x12B20
int32_t d2win_sub_12b20(D2Win12B20Env& e, const uint16_t* s) {
  int32_t n = 0; while (s[n]) ++n;  // Unicode::strlen
  return e.textWidth(s, n);
}
