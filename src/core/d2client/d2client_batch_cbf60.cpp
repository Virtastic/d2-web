// D2Client 0xCBF60..0xCC3D1 batch: MSVC CRT float-conversion helpers as plain C++.
// ponytail: SEH/NLG frame (CBF60) and the string->ld12 parser (FUN_6fb7c903) are Env seams.
#include <cstdint>
#include <cstring>
namespace d2client_batch_cbf60 {
using P = uintptr_t;
struct Env {
 P (*nlgNotify1)(int32_t);                                  // __NLG_Notify1 (0x91E9)
 void (*strToLd12)(uint8_t* ld12, int32_t* out, const uint8_t* s, int, int, int, int); // FUN_6fb7c903
 const int32_t* dblTbl;                                     // DAT_6fbaa5b0 (6 ints)
 const int32_t* fltTbl;                                     // DAT_6fbaa5c8 (6 ints)
};
// 1.13c D2Client+0xCBF60
void sub_CBF60(const Env& e, uint32_t, uint32_t, int32_t p3) {
 P f = e.nlgNotify1(p3);
 ((void (*)())f)();
 if (p3 == 0x100) p3 = 2;
 e.nlgNotify1(p3);
}
// 1.13c D2Client+0xCBFAC
uint32_t sub_CBFAC(const uint32_t* m, int32_t bits) {
 int32_t i = bits / 32;
 uint32_t v = m[i] & ~(0xFFFFFFFFu << ((0x1f - (bits % 32)) & 0x1f));
 for (;;) {
  if (v) return 0;
  if (++i > 2) return 1;
  v = m[i];
 }
}
static uint32_t addl(uint32_t a, uint32_t b, uint32_t* r) { *r = a + b; return *r < a; } // ___addl carry
// 1.13c D2Client+0xCBFDE
uint32_t sub_CBFDE(uint32_t* m, int32_t bits) {
 uint32_t c = addl(m[bits / 32], 1u << ((0x1f - (bits % 32)) & 0x1f), &m[bits / 32]);
 for (int32_t i = bits / 32 - 1; i >= 0; --i) {
  if (!c) return c;
  c = addl(m[i], 1, &m[i]);
 }
 return c;
}
// 1.13c D2Client+0xCC02B
uint32_t sub_CC02B(uint32_t* m, int32_t bits) {
 uint32_t r = 0;
 int32_t i = bits / 32;
 uint32_t sh = (0x1f - (bits % 32)) & 0x1f;
 if ((m[i] & (1u << sh)) && !sub_CBFAC(m, bits)) r = sub_CBFDE(m, bits - 1);
 m[i] &= 0xFFFFFFFFu << sh;
 for (int32_t k = i + 1; k < 3; ++k) m[k] = 0;
 return r;
}
// 1.13c D2Client+0xCC0D1
void sub_CC0D1(uint32_t* m, int32_t bits) {
 uint32_t b = (uint32_t)(bits % 32) & 0xff, carry = 0;
 for (int i = 0; i < 3; ++i) {
  uint32_t v = m[i];
  m[i] = (v >> (b & 0x1f)) | carry;
  carry = (v & ~(0xFFFFFFFFu << (b & 0x1f))) << ((0x20 - b) & 0x1f);
 }
 int32_t w = bits / 32;
 for (int32_t i = 2; i >= 0; --i) m[i] = (i < w) ? 0 : m[i - w];
}
static void copyMan(uint32_t* d, const uint32_t* s) { for (int i = 0; i < 3; ++i) d[i] = s[i]; }
static bool isZeroMan(const uint32_t* m) { return !(m[0] | m[1] | m[2]); }
// 1.13c D2Client+0xCC14C  (t = 6-int format table: maxExp, minExp, mantBits, expShift, outBits, bias)
uint32_t sub_CC14C(const uint8_t* ld12, uint32_t* out, const int32_t* t) {
 uint16_t w0, ex; uint32_t lo, hi;
 memcpy(&w0, ld12, 2); memcpy(&lo, ld12 + 2, 4); memcpy(&hi, ld12 + 6, 4); memcpy(&ex, ld12 + 10, 2);
 uint32_t L[3] = {w0, lo, hi}; // local_8 = w0<<16 sits at m[0]; m = {local_10 ... } see below
 // decompile stack order: local_10 (hi) @m[0], local_c (lo) @m[1], local_8 (w0<<16) @m[2]
 uint32_t m[3] = {hi, lo, (uint32_t)w0 << 16}; (void)L;
 uint32_t sv[3], st;
 int32_t e = (int32_t)(ex & 0x7fff) - 0x3fff;
 if (e == -0x3fff) {
  e = 0;
  if (isZeroMan(m)) { st = 0; goto fin; }
  m[0] = 0; m[1] = 0;
 } else {
  copyMan(sv, m);
  if (sub_CC02B(m, t[2])) e = (int32_t)(ex & 0x7fff) - 0x3ffe;
  if (e < t[1] - t[2]) { m[0] = 0; m[1] = 0; }
  else if (t[1] < e) {
   if (t[0] <= e) { m[1] = 0; m[2] = 0; m[0] = 0x80000000u; sub_CC0D1(m, t[3]); e = t[5] + t[0]; st = 1; goto fin; }
   m[0] &= 0x7fffffffu; e = t[5] + e; sub_CC0D1(m, t[3]); st = 0; goto fin;
  } else {
   copyMan(m, sv); sub_CC0D1(m, t[1] - e); sub_CC02B(m, t[2]); sub_CC0D1(m, t[3] + 1);
  }
 }
 e = 0; st = 2;
fin:
 m[0] = ((uint32_t)e << ((0x1f - t[3]) & 0x1f)) | ((ex & 0x8000) ? 0x80000000u : 0) | m[0];
 if (t[4] == 0x40) { out[1] = m[0]; out[0] = m[1]; }
 else if (t[4] == 0x20) out[0] = m[0];
 return st;
}
// 1.13c D2Client+0xCC2A4
uint32_t sub_CC2A4(const Env& e, const uint8_t* ld12, uint32_t* d) { return sub_CC14C(ld12, d, e.dblTbl); }
// 1.13c D2Client+0xCC2BA
uint32_t sub_CC2BA(const Env& e, const uint8_t* ld12, uint32_t* d) { return sub_CC14C(ld12, d, e.fltTbl); }
// 1.13c D2Client+0xCC2D0
void sub_CC2D0(const Env& e, uint32_t* d, const uint8_t* s) {
 uint8_t ld[12]; int32_t o; e.strToLd12(ld, &o, s, 0, 0, 0, 0); sub_CC2A4(e, ld, d);
}
// 1.13c D2Client+0xCC315
void sub_CC315(const Env& e, uint32_t* d, const uint8_t* s) {
 uint8_t ld[12]; int32_t o; e.strToLd12(ld, &o, s, 0, 0, 0, 0); sub_CC2BA(e, ld, d);
}
// 1.13c D2Client+0xCC35A  (strflt: +4 decpt, +0xc digit string)
void sub_CC35A(char* buf, int32_t n, uint8_t* strflt) {
 char* digs; memcpy(&digs, strflt + 0xc, sizeof digs);
 char* s = buf + 1; char* p = s;
 *buf = '0';
 if (n > 0) { for (int32_t k = n; k; --k) { char c = *digs; if (!c) c = '0'; else ++digs; *p++ = c; } n = 0; }
 *p = 0;
 if (n >= 0 && *digs > '4') { while (*--p == '9') *p = '0'; ++*p; }
 if (*buf == '1') { int32_t d; memcpy(&d, strflt + 4, 4); ++d; memcpy(strflt + 4, &d, 4); }
 else memmove(buf, s, strlen(s) + 1);
}
// 1.13c D2Client+0xCC3D1  (double -> 80-bit long double, 10 bytes at dst)
void sub_CC3D1(uint32_t* dst, const uint32_t* src) {
 uint16_t top = (uint16_t)(src[1] >> 16), ex = (top >> 4) & 0x7ff;
 uint32_t lo = src[0], flag = 0x80000000u;
 if (!ex) {
  if (!(src[1] & 0xfffff) && !lo) { dst[0] = dst[1] = 0; memset(&dst[2], 0, 2); return; }
  ex += 0x3c01; flag = 0;
 } else if (ex == 0x7ff) ex = 0x7fff; else ex += 0x3c00;
 dst[1] = (lo >> 21) | ((src[1] & 0xfffff) << 11) | flag;
 dst[0] = lo << 11;
 while (!flag) {
  uint32_t v = dst[1]; --ex;
  dst[1] = (v << 1) | (dst[0] >> 31); dst[0] <<= 1; flag = (v << 1) & 0x80000000u;
 }
 uint16_t r = (top & 0x8000) | ex; memcpy(&dst[2], &r, 2);
}
} // namespace d2client_batch_cbf60
