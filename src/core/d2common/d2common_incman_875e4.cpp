// D2Common 1.13c: CRT __IncMan; returns the last ___addl carry (EAX at exit, read by __RoundMan).
#include <cstdint>
#include <emscripten.h>
extern "C" {
static int addl(uint32_t a, uint32_t b, uint32_t* o) { uint32_t s = a + b; *o = s; return s < a; }

// 1.13c D2Common+0x875E4
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_875e4(uint32_t* m, int32_t bit) {
  uint32_t* p = m + bit / 32;
  int32_t c = addl(*p, 1u << ((31u - (uint32_t)(bit % 32)) & 31), p);
  for (int32_t i = bit / 32 - 1; i >= 0; i--) {
    if (!c) return c;
    c = addl(m[i], 1, &m[i]);
  }
  return c;
}
}
