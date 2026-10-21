// D2Common 1.13c batch. Retail global DAT_6fde36a8 is passed as tbl: 10 entries of 5 int32
// (+0 kind, +4 lo, +8 hi, +12 base, +16 alt base).
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x40C20: map value p3 of kind p2 (0x30/0x31) to a base; alt base if unit+0x1D0 == 0x75.
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_40c20(const uint8_t* unit, int32_t p2, int32_t p3, const int32_t* tbl) {
 if (p2 != 0x30 && p2 != 0x31) __builtin_trap();
 const int32_t* e = tbl;
 while (p2 != e[0] || p3 < e[1] || e[2] < p3) {
  e += 5;
  if (e >= tbl + 50) __builtin_trap();
 }
 const int32_t u = *reinterpret_cast<const int32_t*>(unit + 0x1D0);
 return (u == 0x75 ? e[4] : e[3]) - e[1] + p3;
}
}
