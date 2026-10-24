// D2Common 1.13c: CRT __RoundMan. __ZeroTail/__IncMan are the sibling funcs 0x875B2/0x875E4.
#include <cstdint>
#include <emscripten.h>
extern "C" {
int32_t d2common_zerotail_875b2(const uint32_t*, int32_t);
int32_t d2common_incman_875e4(uint32_t*, int32_t);
// 1.13c D2Common+0x87631
EMSCRIPTEN_KEEPALIVE int32_t d2common_roundman_87631(uint32_t* m, int32_t bit) {
  int32_t r = 0, i = bit / 32;
  uint32_t sh = (31u - (uint32_t)(bit % 32)) & 31;
  if ((m[i] & (1u << sh)) && !d2common_zerotail_875b2(m, bit)) r = d2common_incman_875e4(m, bit - 1);
  m[i] &= ~0u << sh;
  for (i++; i < 3; i++) m[i] = 0;
  return r;
}
}
