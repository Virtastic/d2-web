// D2Common batch: ld12 scaler by power-of-ten tables (positive table 0x6fdef280, negative 0x6fdef3e0).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" {
// Unwired callee (weak): ld12 multiply +0x8850F; tables are 0x160 bytes apart.
__attribute__((weak)) void d2common_mul_ext_8850f(uint8_t* pA, const uint8_t* pB);
__attribute__((weak)) uint8_t d2common_dat_6fdef280[0x160];
__attribute__((weak)) uint8_t d2common_dat_6fdef3e0[0x160];

// 1.13c D2Common+0x88745
EMSCRIPTEN_KEEPALIVE void D2Common_88745(uint32_t* ld12, int32_t exp10, int32_t p4) {
 const uint8_t* tbl = d2common_dat_6fdef280;
 uint32_t n = (uint32_t)exp10;
 if (n == 0) return;
 if (exp10 < 0) { n = -n; tbl = d2common_dat_6fdef3e0; }
 if (p4 == 0) *(uint16_t*)ld12 = 0;
 while (n != 0) {
  const uint32_t r = n & 7;
  n = (uint32_t)((int32_t)n >> 3);
  tbl += 0x54;
  if (r != 0) {
   const uint8_t* e = tbl + r * 0xc;
   uint8_t tmp[12];
   uint16_t w;
   memcpy(&w, e, 2);
   if (w > 0x7fff) {
    memcpy(tmp, e, 12);
    uint32_t d; memcpy(&d, tmp + 2, 4); d -= 1; memcpy(tmp + 2, &d, 4);
    e = tmp;
   }
   d2common_mul_ext_8850f((uint8_t*)ld12, (const uint8_t*)e);
  }
 }
}
}
