// D2Common 1.13c: CRT __ShrMan (3-word mantissa right shift by param_2 bits).
#include <cstdint>
#include <emscripten.h>
extern "C" {
// 1.13c D2Common+0x876D7
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_876d7(uint32_t* m, int32_t n) {
  uint32_t b = (uint32_t)(n % 32), carry = 0;
  for (int i = 0; i < 3; i++) {
    uint32_t v = m[i];
    m[i] = (v >> (b & 31)) | carry;
    carry = (v & ~(~0u << (b & 31))) << ((32u - b) & 31);
  }
  int32_t w = n / 32;
  for (int i = 2; i >= 0; i--) m[i] = i < w ? 0 : m[2 - w - (2 - i)];
}
}
