// D2Common 1.13c batch: CRT ___dtold (double -> 80-bit long double).
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x879D7: out = 12-byte x87 extended (lo, hi, se), in = 8-byte double.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_879d7(uint32_t out[3], const uint32_t in[2]) {
 const uint16_t hi = static_cast<uint16_t>(in[1] >> 16);
 uint32_t top = 0x80000000u;
 uint16_t e = (hi >> 4) & 0x7ff;
 const uint32_t lo = in[0];
 if (e == 0) {
  if ((in[1] & 0xfffff) == 0 && lo == 0) { out[1] = 0; out[0] = 0; *reinterpret_cast<uint16_t*>(out + 2) = 0; return; }
  e = static_cast<uint16_t>(e + 0x3c01);
  top = 0;
 } else if (e == 0x7ff) e = 0x7fff;
 else e = static_cast<uint16_t>(e + 0x3c00);
 out[1] = (lo >> 21) | ((in[1] & 0xfffff) << 11) | top;
 out[0] = lo << 11;
 while (top == 0) {
  const uint32_t t = out[1];
  e = static_cast<uint16_t>(e - 1);
  out[1] = (t << 1) | (out[0] >> 31);
  out[0] = out[0] * 2;
  top = (t << 1) & 0x80000000u;
 }
 *reinterpret_cast<uint16_t*>(out + 2) = static_cast<uint16_t>((hi & 0x8000) | e);
}
}
