#include <cstdint>
#include <cstdlib>
// 1.13c D2Client+0x38310 : numeric edit-box value; 0 if empty or above max.
extern "C" uint32_t d2c_edit_value_38310(const uint16_t* text, uint32_t maxChars, int32_t maxVal) {
  if (!text || !text[0]) return 0;
  char b[100]; uint32_t i = 0;
  for (; i < maxChars && i < 99 && text[i]; i++) b[i] = (char)text[i];
  b[i] = 0;
  int32_t v = (int32_t)atol(b);
  return maxVal < v ? 0 : (uint32_t)v;
}
