// D2Common 1.13c batch: CRT %g-style float-to-text (_cftog): picks %e or %f form by exponent.
#include <cstdint>
#include <emscripten.h>

extern "C" {
__attribute__((weak)) intptr_t d2_fn_d2common_87a91(uint32_t flt, uint32_t p2, int32_t* strflt, uint32_t* buf6);  // FUN_6fdd7a91
__attribute__((weak)) intptr_t d2_fn_d2common_87960(char* buf, uint32_t size, int32_t ndig, uint32_t strflt);      // __fptostr
__attribute__((weak)) intptr_t d2_fn_d2common_87344(const int32_t* strflt, char* buf, uint32_t ndec, char g);         // __cftof2
__attribute__((weak)) void d2_fn_d2common_87226(intptr_t flt, intptr_t buf, int32_t nDigits, int32_t bCaps, char bG);  // __cftoe2

// 1.13c D2Common+0x87447: p1 -> {double lo, hi}; p2 = output text, p3 = precision, p4 = caps flag.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_87447(const uint32_t* p1, char* p2, uint32_t p3, int32_t p4) {
 int32_t st[4];  // +0 sign, +4 decpt
 uint32_t tmp[6];
 const uint32_t flt = p1[0];
 d2_fn_d2common_87a91(flt, p1[1], st, tmp);
 const int32_t e0 = st[1] - 1;
 char* const body = p2 + (st[0] == 0x2d);
 d2_fn_d2common_87960(body, p3, reinterpret_cast<intptr_t>(st), flt);
 const int32_t e = st[1] - 1;
 st[1] = e;
 if (e < -4 || static_cast<int32_t>(p3) <= e) {
  d2_fn_d2common_87226(reinterpret_cast<intptr_t>(st), reinterpret_cast<intptr_t>(p2), static_cast<int32_t>(p3), p4, 1);
 } else {
  if (e0 < e) {
   char* q = body;
   while (*q++ != '\0') {}
   q[-2] = '\0';  // retail drops the last digit when rounding raised the exponent
  }
  d2_fn_d2common_87344(st, p2, p3, 1);
 }
}
}
