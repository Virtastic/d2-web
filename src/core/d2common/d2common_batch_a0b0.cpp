// D2Common 1.13c batch. Retail global DAT_6fdee2c8 = table of 600-byte rows (150 ints each).
#include <cstdint>
#include <emscripten.h>

extern "C" {
// weak: owning definition lives elsewhere (retail global at 0x6fdee2c8), 256 rows of 150 ints.
__attribute__((weak)) int32_t d2_g_6fdee2c8[256 * 150];

// 1.13c D2Common+0xA0B0: count leading non-zero ints in row `row` (scanned 6 at a time, up to 156).
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_a0b0(uint8_t row) {
 const int32_t* p = d2_g_6fdee2c8 + row * 150 + 1;
 int32_t n = 0;
 for (int32_t k = 0; k <= 0x95; k += 6, p += 6, n += 6)
  for (int32_t j = 0; j < 6; ++j)
   if (p[j - 1] == 0) return n + j;
 return n;
}
}
