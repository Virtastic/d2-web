// D2Common 1.13c: CRT __ZeroTail (3-word mantissa, bit index in param_2).
#include <cstdint>
#include <emscripten.h>
extern "C" {
// 1.13c D2Common+0x875B2
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_875b2(const uint32_t* m, int32_t bit) {
  int32_t i = bit / 32;
  uint32_t v = m[i] & ~(~0u << ((31u - (uint32_t)(bit % 32)) & 31));
  for (;;) {
    if (v) return 0;
    if (++i > 2) return 1;
    v = m[i];
  }
}
}
