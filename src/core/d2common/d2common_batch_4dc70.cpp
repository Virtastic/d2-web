// D2Common 1.13c batch: recursive diminishing-returns interpolation between lo and hi.
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x4DC70
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_4dc70(int32_t p1, int32_t lo, int32_t hi) {
 int32_t v = ((p1 * 0x6e) / (p1 + 6)) * (hi - lo) / 100 + lo;
 if (v <= ((p1 * 0x6e - 0x6e) / (p1 + 5)) * (hi - lo) / 100 + 1 + lo)
  return d2_fn_d2common_4dc70(p1 - 1, lo, hi) + 1;
 return hi < v ? hi : v;
}
}
