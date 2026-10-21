// D2Common 1.13c batch: CRT __fptostr (digit string rounding into a buffer).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x87960: ndig is retail's STRFLT pointer (+4 decpt, +0xc digit text), passed as int32 by callers.
EMSCRIPTEN_KEEPALIVE intptr_t d2_fn_d2common_87960(char* buf, uint32_t size, int32_t ndig, uint32_t strflt) {
 (void)strflt;
 int32_t* const st = reinterpret_cast<int32_t*>(static_cast<uintptr_t>(static_cast<uint32_t>(ndig)));
 const char* src = reinterpret_cast<const char*>(static_cast<uintptr_t>(static_cast<uint32_t>(st[3])));
 char* const dst = buf;
 char* const str = buf + 1;
 char* p = str;
 *buf = '0';
 int32_t n = static_cast<int32_t>(size);
 if (0 < n) {
  do {
   char c = *src;
   if (c == '\0') c = '0'; else ++src;
   *p++ = c;
  } while (--n != 0);
 }
 *p = '\0';
 if (0 <= static_cast<int32_t>(size) && '4' < *src) {
  while (*--p == '9') *p = '0';
  *p = static_cast<char>(*p + 1);
 }
 if (*dst == '1') { st[1] += 1; return reinterpret_cast<intptr_t>(p); }
 return reinterpret_cast<intptr_t>(memmove(dst, str, strlen(str) + 1));
}
}
