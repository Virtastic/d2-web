// D2Common 1.13c batch: MSVC 2003 CRT __cftof2 (%f / %g fixed formatting helper).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" {
// Unwired callees: __shift (+0x87209; retail EAX=string, EDI=offset) and the locale decimal point.
__attribute__((weak)) void D2Common_87209_Shift(char* pStr, int32_t nOffset);
__attribute__((weak)) char d2common_dat_6fde9964[1];

// 1.13c D2Common+0x87344: strflt (retail EAX) = {+0 sign char, +4 decimal position}.
EMSCRIPTEN_KEEPALIVE intptr_t d2_fn_d2common_87344(const int32_t* strflt, char* buf, uint32_t ndec, char g) {
 const int32_t dp = strflt[1];
 const bool neg = strflt[0] == 0x2d;
 if (g && static_cast<uint32_t>(dp - 1) == ndec) {
  buf[(neg ? 1u : 0u) + static_cast<uint32_t>(dp - 1)] = '0';
  buf[(neg ? 1u : 0u) + static_cast<uint32_t>(dp - 1) + 1] = 0;
 }
 char* p = buf;
 if (neg) *p++ = '-';
 if (strflt[1] < 1) {
  D2Common_87209_Shift(p, 1);
  *p++ = '0';
 } else {
  p += strflt[1];
 }
 if (static_cast<int32_t>(ndec) > 0) {
  D2Common_87209_Shift(p, 1);
  *p = d2common_dat_6fde9964[0];
  if (strflt[1] < 0) {
   uint32_t n = static_cast<uint32_t>(-strflt[1]);
   if (g || static_cast<int32_t>(n) <= static_cast<int32_t>(ndec)) ndec = n;
   D2Common_87209_Shift(p + 1, static_cast<int32_t>(ndec));
   std::memset(p + 1, '0', ndec);
  }
 }
 return reinterpret_cast<intptr_t>(buf);
}
}
