// D2Common 1.13c batch. Units are 32-bit retail images; embedded pointers (unit+0x2C path) are 32-bit values.
#include <cstdint>
#include <emscripten.h>

extern "C" {
__attribute__((weak)) int32_t d2_sub_7FCD0(const int32_t* u1, const int32_t* u2) { (void)u1; (void)u2; return 0; }
__attribute__((weak)) int32_t d2_sub_30F80(const int32_t* u) { (void)u; return 0; }
__attribute__((weak)) int32_t d2_sub_30EE0(const int32_t* u) { (void)u; return 0; }

static inline bool PathIsLong(int32_t t) { return t == 2 || (t > 3 && t < 6); }
static inline void Pos(const int32_t* u, int32_t& x, int32_t& y) {
 const int32_t* p = reinterpret_cast<const int32_t*>(static_cast<uintptr_t>(static_cast<uint32_t>(u[0xb])));
 const uint16_t* s = reinterpret_cast<const uint16_t*>(p);
 if (PathIsLong(u[0])) { x = p[3]; y = p[4]; }
 else if (!p) { x = 0; y = 0; }
 else { x = s[1]; y = s[3]; }
}

// 1.13c D2Common+0x31000: is unit u2 (must be type 2) within one/two cells of unit u1's footprint.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_31000(const int32_t* u1, const int32_t* u2) {
 if (!u2 || u2[0] != 2) return 0;
 if (d2_sub_7FCD0(u1, u2) == 0) return 1;
 int32_t ax, ay, bx, by;
 Pos(u1, ax, ay);
 Pos(u2, bx, by);
 const int32_t w = d2_sub_30F80(u2), h = d2_sub_30EE0(u2);
 const int32_t x0 = bx - w / 2, y0 = by - h / 2;
 if (w < 1 || h < 1) {
  if (ax < x0 - 1 || x0 + 1 < ax || ay < y0 - 1) return 0;
  return ay <= y0 + 1;
 }
 if (ax < x0 - 2 || x0 + w + 2 < ax || ay < y0 - 2 || y0 + h + 2 < ay) return 0;
 if (static_cast<uint32_t>(d2_sub_30F80(u1)) > 2) return 1;
 if (ay < y0 - 1) return ax >= x0 - 1 && ax <= x0 + w + 1;
 if (ay <= y0 + h + 1) return 1;
 return ax >= x0 - 1 && ax <= x0 + w + 1;
}
}
