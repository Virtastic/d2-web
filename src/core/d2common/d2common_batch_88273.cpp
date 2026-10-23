// D2Common 1.13c batch: CRT 80-bit long double -> decimal digit record (_fltout2 core).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
void d2common_shl_12_87cf0(uint32_t* words);   // d2common_batch_9330.cpp (C++ linkage)
void d2common_shr_12_87d1e(uint32_t* words);   // d2common_batch_9330.cpp (C++ linkage)

extern "C" {
void D2Common_87C92_Add12(uint32_t* pA, const uint32_t* pB);
void D2Common_88745(uint32_t* ld12, int32_t exp10, int32_t p4);
void d2common_mul_ext_8850f(uint8_t* pA, const uint8_t* pB);

// rec: [0]=i16 decpt, [2]=sign char, [3]=digit count, [4..]=NUL-terminated digits.
// 1.13c D2Common+0x88273
EMSCRIPTEN_KEEPALIVE intptr_t d2_fn_d2common_88273(uint32_t lo, uint32_t hi, uint32_t se, int32_t ndig, int32_t p5, void* recv) {
 uint8_t* rec = static_cast<uint8_t*>(recv);
 uint8_t* dig = rec + 4;
 const uint32_t ex = se & 0x7fff;
 const int neg = (se & 0x8000) != 0;
 alignas(4) uint8_t k[12] = {0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xfb,0x3f};  // 12-byte constant fed to the 8850F multiply
 alignas(4) uint8_t b[12];  // [0..1]=0, [2..5]=lo, [6..9]=hi, [10..11]=exponent
 uint32_t* w = reinterpret_cast<uint32_t*>(b);
 rec[2] = neg ? '-' : ' ';
 if (ex == 0 && hi == 0 && lo == 0) {
 zero:
  dig[0] = '0';
 zero2:
  memset(rec, 0, 2);
  rec[2] = ' '; rec[3] = 1; rec[5] = 0;
  return 1;
 }
 if (ex == 0x7fff) {
  const char* s;
  *reinterpret_cast<int16_t*>(rec) = 1;
  if ((hi == 0x80000000u && lo == 0) || (hi & 0x40000000u)) {
   if (!neg || hi != 0xc0000000u) { if (hi != 0x80000000u || lo != 0) { s = "1#QNAN"; } else { s = "1#INF"; } }
   else if (lo != 0) { s = "1#QNAN"; }
   else { s = "1#IND"; }
   rec[3] = 5;
  } else { s = "1#SNAN"; rec[3] = 6; }
  strcpy(reinterpret_cast<char*>(dig), s);
  return 0;
 }
 int16_t d = static_cast<int16_t>(((((ex >> 8) + (hi >> 24) * 2) * 0x4d + 0xecbced0cu + ex * 0x4d10) >> 16));
 memset(b, 0, 2);
 memcpy(b + 2, &lo, 4); memcpy(b + 6, &hi, 4);
 b[10] = static_cast<uint8_t>(ex); b[11] = static_cast<uint8_t>(ex >> 8);
 D2Common_88745(w, -static_cast<int32_t>(d), 1);
 uint16_t e16 = static_cast<uint16_t>(b[10] | (b[11] << 8));
 if (e16 > 0x3ffe) { d++; d2common_mul_ext_8850f(b, k); }
 *reinterpret_cast<int16_t*>(rec) = d;
 if ((p5 & 1) && (ndig += d, ndig < 1)) goto zero;
 if (ndig > 0x15) ndig = 0x15;
 const int e = (b[10] | (b[11] << 8)) - 0x3ffe;
 b[10] = b[11] = 0;
 for (int i = 0; i < 8; i++) d2common_shl_12_87cf0(w);
 if (e < 0) for (uint32_t i = (-e) & 0xff; i; i--) d2common_shr_12_87d1e(w);
 int cnt = ndig + 1, n = 0;
 for (; cnt > 0; cnt--) {
  uint32_t c[3]; memcpy(c, b, 12);
  d2common_shl_12_87cf0(w); d2common_shl_12_87cf0(w);
  D2Common_87C92_Add12(w, c);
  d2common_shl_12_87cf0(w);
  dig[n++] = static_cast<uint8_t>(b[11] + '0');
  b[11] = 0;
 }
 int p = n - 1;
 if (dig[p] < '5') {
  while (p >= 0 && dig[p] == '0') p--;
  if (p < 0) { dig[0] = '0'; goto zero2; }
 } else {
  while (p >= 0 && dig[p] == '9') dig[p--] = '0';
  if (p < 0) { p = 0; (*reinterpret_cast<int16_t*>(rec))++; }
  dig[p]++;
 }
 rec[3] = static_cast<uint8_t>(p + 1);
 dig[p + 1] = 0;
 return 1;
}
}
