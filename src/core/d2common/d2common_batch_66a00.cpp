// D2Common 1.13c batch: 0x66A00 scaled direction vector (ESI=out[2], EBX=scale are register args).
#include <cstdint>
#include <emscripten.h>

extern "C" {
// Unwired callees (weak so the file links): 0x5CF40 direction helper, ordinals 10265/10024 error log.
__attribute__((weak)) void d2_sub_5cf40(int32_t, int32_t, int32_t, int32_t, int32_t*, uint32_t*);
__attribute__((weak)) uint32_t d2_ord_10265(int32_t);
__attribute__((weak)) void d2_ord_10024(const void*, uint32_t);

// 1.13c D2Common+0x66A00
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_66a00(uint32_t p1, uint32_t p2, int32_t* out, int32_t scale) {
 if (d2_sub_5cf40) d2_sub_5cf40(int32_t(p1*0x10000+0x8000), int32_t((p1&0xffff0000)+0x8000),
  int32_t(p2*0x10000+0x8000), int32_t((p2&0xffff0000)+0x8000), out, &p1);
 int32_t y = (out[1]*scale) >> 8, x = (out[0]*scale) >> 8;
 out[0] = x; out[1] = y;
 if (y < 0) y = -y;
 if (x < 0) x = -x;
 if (x <= y) x = y;
 if (x > 0x1fffff) {
  if (d2_ord_10265 && d2_ord_10024) d2_ord_10024(nullptr, d2_ord_10265(0x331));
  __builtin_trap();
 }
}
}
