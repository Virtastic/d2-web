// D2Common 1.13c batch: MSVC 2003 CRT __cftoe2 (exponent-format tail of printf %e).
#include <cstdint>
#include <emscripten.h>

extern "C" {
// Unwired callees: +0x87209 __shift(str, offset), +0x6960 strcpy; DAT_6fde9964 decimal-point char.
__attribute__((weak)) void D2Common_87209_Shift(char* pStr, int32_t nOffset);
__attribute__((weak)) uint32_t* D2Common_6960_Strcpy(uint32_t* pDest, const uint32_t* pSrc);
__attribute__((weak)) char d2common_dat_6fde9964[1];

// 1.13c D2Common+0x87226: in_EAX = strflt {+0 sign char, +4 decpt, +c digits ptr}, EBX = buf.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_87226(intptr_t flt, intptr_t buf, int32_t nDigits, int32_t bCaps, char bG) {
 const int32_t* f = reinterpret_cast<const int32_t*>(flt);
 char* p = reinterpret_cast<char*>(buf);
 if (bG != 0) D2Common_87209_Shift(p, 1);
 if (f[0] == 0x2d) *p++ = 0x2d;
 char* q = p;
 if (0 < nDigits) {
  q = p + 1;
  *p = *q;
  *q = d2common_dat_6fde9964[0];
 }
 char* e = reinterpret_cast<char*>(D2Common_6960_Strcpy(reinterpret_cast<uint32_t*>(q + nDigits + (bG == 0 ? 1 : 0)), reinterpret_cast<const uint32_t*>("e+000")));
 if (bCaps != 0) *e = 0x45;
 if (*reinterpret_cast<const char*>(static_cast<intptr_t>(f[3])) != '0') {
  int32_t x = f[1] - 1;
  if (x < 0) { x = -x; e[1] = 0x2d; }
  if (99 < x) { e[2] = static_cast<char>(e[2] + x / 100); x %= 100; }
  if (9 < x) { e[3] = static_cast<char>(e[3] + x / 10); x %= 10; }
  e[4] = static_cast<char>(e[4] + x);
 }
}
}
