// D2Client 0xCC7EE..0xCD5C2 batch: statically linked MSVC CRT x87 80-bit helpers (shr/shl/add_12, strtold-style
// parse, fltout digit generation, 10^n scaling, signal raise) as plain C++ over host memory.
// Pointer args are uintptr_t to a 12-byte "ext" (u16 guard@0, u64 mantissa@2, u16 sign|exp@10) or 3 x u32.
// Unrecovered callees / CRT globals are Env members (approximate where Env cannot express the ptd/lock state).
#include <cstdint>
#include <cstring>
namespace d2client_batch_cc7ee {
using P = uintptr_t;
struct Env {
 uint8_t decimalPoint;          // DAT_6fb8b914
 P pow10Pos, pow10Neg;          // 0x6fbaa580 / 0x6fbaa6e0 power tables (12-byte entries)
 uint32_t (*rd32)(P); void (*wr32)(P, uint32_t);
 P sigTable; uint32_t sigCount; // DAT_6fb8b83c count; exception-action table (12-byte rows, signum @+4, handler @+8)
 P sigIntSlot, sigTermSlot, sigBreakSlot, sigAbortSlot; // DAT_6fbd3718/24/1c/20 handler slots (SIGINT,15,21,22)
 void (*callHandler)(P fn, int sig, int sub); void (*exitFn)(int);
 P flag3700;                    // DAT_6fbd3700
 void (*f1000)(P);              // FUN_6fab1000
};
static uint32_t* W(P p) { return reinterpret_cast<uint32_t*>(p); }
static int addl(uint32_t a, uint32_t b, uint32_t* o) { uint32_t s = a + b; *o = s; return s < a || s < b; }
static void shl12(uint32_t* p) { uint32_t a = p[0], b = p[1]; p[0] = a << 1; p[1] = b << 1 | a >> 31; p[2] = p[2] << 1 | b >> 31; }
static void add12(uint32_t* d, const uint32_t* s) {
 if (addl(d[0], s[0], d)) { if (addl(d[1], 1, d + 1)) d[2]++; }
 if (addl(d[1], s[1], d + 1)) d[2]++;
 addl(d[2], s[2], d + 2);
}
static uint16_t ld16(const void* p) { uint16_t v; memcpy(&v, p, 2); return v; }
static void st16(void* p, uint16_t v) { memcpy(p, &v, 2); }
static uint64_t ldm(const uint8_t* e) { uint64_t v; memcpy(&v, e + 2, 8); return v; }
static void stm(uint8_t* e, uint64_t v) { memcpy(e + 2, &v, 8); }

// 1.13c D2Client+0xCC7EE  ___shr_12
void sub_CC7EE(P a) { uint32_t* p = W(a); uint32_t t = p[1]; p[1] = t >> 1 | p[2] << 31; p[2] >>= 1; p[0] = p[0] >> 1 | t << 31; }

// 1.13c D2Client+0xCC81B  decimal digits -> unnormalised ext (digits are raw 0..9 bytes)
void sub_CC81B(P digits, int n, P out) {
 const char* s = reinterpret_cast<const char*>(digits); uint32_t* o = W(out); int16_t ex = 0x404e;
 o[0] = o[1] = o[2] = 0;
 for (; n; n--, s++) { uint32_t c[3] = {o[0], o[1], o[2]}; shl12(o); shl12(o); add12(o, c); shl12(o); uint32_t d[3] = {(uint32_t)(int32_t)*s, 0, 0}; add12(o, d); }
 if (o[2] == 0) { uint32_t hi; do { ex -= 0x10; hi = o[1] >> 16; o[1] = o[0] >> 16 | o[1] << 16; o[0] <<= 16; } while (hi == 0); o[2] = hi; }
 while (!(o[2] & 0x8000)) { shl12(o); ex--; }
 st16(reinterpret_cast<uint8_t*>(out) + 10, (uint16_t)ex);
}

// 1.13c D2Client+0xCD186  ext multiply a *= b (round to nearest even on the 128-bit product)
void sub_CD186(P pa, P pb) {
 uint8_t* a = reinterpret_cast<uint8_t*>(pa); const uint8_t* b = reinterpret_cast<const uint8_t*>(pb);
 uint16_t ea = ld16(a + 10) & 0x7fff, eb = ld16(b + 10) & 0x7fff, sg = (ld16(a + 10) ^ ld16(b + 10)) & 0x8000;
 uint16_t e = ea + eb;
 if (ea < 0x7fff && eb < 0x7fff && e < 0xbffe) {
  if (e < 0x3fc0 || ldm(a) == 0 || ldm(b) == 0) { memset(a, 0, 12); return; }
  unsigned __int128 pr = (unsigned __int128)ldm(a) * ldm(b);
  int32_t ex = (int32_t)e - 0x3ffe;
  uint64_t hi = (uint64_t)(pr >> 64), lo = (uint64_t)pr;
  while (!(hi >> 63) && ex > 0) { hi = hi << 1 | lo >> 63; lo <<= 1; ex--; }
  if (ex <= 0) { int sh = 1 - ex; if (sh > 63) { hi = 0; ex = 0; } else { lo = (lo >> sh) | (hi << (64 - sh)) | ((lo << (64 - sh)) != 0); hi >>= sh; ex = 0; } }
  // ponytail: ties-to-even on the top 64 bits; retail rounds on 16-bit words, identical except pathological sticky cases
  if (lo > (1ull << 63) || (lo == (1ull << 63) && (hi & 1))) { if (++hi == 0) { hi = 1ull << 63; ex++; } }
  if (ex > 0x7ffe) { memset(a, 0, 12); st16(a + 10, 0); W(pa)[2] = (sg ? 0xffff8000u : 0x7fff8000u); return; }
  st16(a, 0); stm(a, hi); st16(a + 10, (uint16_t)ex | sg);
 } else { W(pa)[0] = W(pa)[1] = 0; W(pa)[2] = (sg ? 0x80000000u : 0) + 0x7fff8000u; }
}

// 1.13c D2Client+0xCD3BC  ext *= 10^n via the 8-way power tables; `keep`==0 zeroes the guard word first
void sub_CD3BC(const Env& e, P x, int32_t n, int keep) {
 P tb = e.pow10Pos; uint32_t u = (uint32_t)n;
 if (!n) return;
 if (n < 0) { u = (uint32_t)-n; tb = e.pow10Neg; }
 if (!keep) st16(reinterpret_cast<void*>(x), 0);
 while (u) {
  uint32_t d = u & 7; u = (uint32_t)((int32_t)u >> 3); tb += 0x54;
  if (!d) continue;
  P ent = tb + d * 12; uint8_t tmp[12];
  if (ld16(reinterpret_cast<void*>(ent)) > 0x7fff) { memcpy(tmp, reinterpret_cast<void*>(ent), 12); uint32_t m; memcpy(&m, tmp + 2, 4); m--; memcpy(tmp + 2, &m, 4); ent = reinterpret_cast<P>(tmp); }
  sub_CD186(x, ent);
 }
}

// 1.13c D2Client+0xCC903  strtold-style scan (_fltin2). returns 0 ok / 1 underflow / 2 overflow / 4 no digits
uint32_t sub_CC903(const Env& e, P out, P endp, P str, int a4, int a5, int a6, int a7) {
 const uint8_t *s = reinterpret_cast<const uint8_t*>(str), *s0, *q;
 int st = 0, sgn = 1, exp10 = 0, nd = 0, dadj = 0, sawE = 0, sawDot = 0, sawEz = 0; uint32_t ret = 0; uint16_t sign = 0;
 char dig[25] = {0}; char* dp = dig; uint8_t c; uint8_t T[12] = {0};
 auto isd = [](uint8_t ch) { return ch >= '0' && ch <= '9'; };
 s0 = s;
 while (*s == ' ' || *s == 9 || *s == 10 || *s == 13) s++;
 bool have = false;
 for (;;) {
  c = *s; q = s + 1;
  switch (st) {
  case 0:
   if (c > '0' && c <= '9') { st = 3; continue; }
   if (c == e.decimalPoint) { st = 5; s = q; continue; }
   st = 2;
   if (c == '+') { sign = 0; s = q; continue; }
   if (c == '-') { sign = 0x8000; s = q; continue; }
   if (c != '0') goto done; st = 1; s = q; continue;
  case 1:
   have = true;
   if (c > '0' && c <= '9') { st = 3; continue; }
   if (c == e.decimalPoint) goto dot;
   if (c == '+' || c == '-') goto sgnE;
   s = q; if (c != '0') goto chkE; continue;
  case 2:
   if (c > '0' && c <= '9') { st = 3; continue; }
   s = s0 ? s0 : s; // retail restores the pre-sign position (local_44)
   if (c == e.decimalPoint) { st = 5; s = q; continue; }
   if (c != '0') goto done; st = 1; s = q; continue;
  case 3:
   while (isd(c)) { if (nd < 25) { nd++; *dp++ = (char)(c - '0'); } else dadj++; c = *q++; }
   if (c != e.decimalPoint) goto afterDigits;
  dot: have = true; st = 4; s = q; continue;
  case 4:
   sawDot = 1;
   if (nd == 0) while (c == '0') { dadj--; c = *q++; }
   while (isd(c)) { if (nd < 25) { nd++; *dp++ = (char)(c - '0'); dadj--; } c = *q++; }
  afterDigits:
   if (c == '+' || c == '-') { sgnE: have = true; st = 11; s = q - 1; continue; }
   have = true;
   if (c < 'D' || (c > 'E' && (c < 'd' || c > 'e'))) { s = q - 1; goto done; }
   st = 6; s = q; continue;
  case 5:
   sawDot = 1;
   if (isd(c)) { st = 4; s = q - 1; continue; }
   goto done;
  case 6:
   if (c < '1' || c > '9') {
    if (c == '+') { st = 7; s = q; continue; }
    if (c == '-') { sgn = -1; st = 7; s = q; continue; }
    if (c == '0') { st = 8; s = q; continue; }
    s = q - 1; goto done;
   }
   st = 9; s = q - 1; continue;
  case 7:
   if (c > '0' && c <= '9') { st = 9; s = q - 1; continue; }
   if (c != '0') { s = q - 1; goto done; }
   st = 8; s = q; continue;
  case 8:
   sawEz = 1;
   while (c == '0') c = *q++;
   if (c > '0' && c <= '9') { st = 9; s = q - 1; continue; }
   s = q - 1; goto done;
  case 9: {
   sawEz = 1; int ev = 0;
   while (isd(c)) { ev = (c - '0') + ev * 10; if (ev > 0x1450) { ev = 0x1451; break; } c = *q++; }
   while (isd(c)) c = *q++;
   s = q - 1; exp10 = ev; goto done;
  }
  case 0xb:
   if (a7) { if (c == '+' || c == '-') { if (c == '-') sgn = -1; st = 7; s = q; continue; } goto done; }
   goto done;
  default: goto done;
  }
 chkE:
  have = true;
  if (c < 'D' || (c > 'E' && (c < 'd' || c > 'e'))) { s = q - 1; goto done; }
  st = 6;
 }
done:
 (void)sawE; (void)sawDot;
 *reinterpret_cast<P*>(endp) = reinterpret_cast<P>(s);
 uint8_t R[12] = {0};
 if (!have) ret = 4;
 else if (nd) {
  if (nd > 24) { if (dig[23] > 4) dig[23]++; dp--; dadj++; nd = 24; }
  while (nd && *--dp == 0) { nd--; dadj++; }
  if (nd) {
   uint32_t o3[3]; sub_CC81B(reinterpret_cast<P>(dig), nd, reinterpret_cast<P>(o3));
   memcpy(T, o3, 12);
   int32_t ev = sgn < 0 ? -exp10 : exp10; int32_t x = ev + dadj;
   if (!sawEz) x += a5;
   if (!sawDot) x -= a6;
   if (x > 0x1450) { memset(R, 0, 12); st16(R + 10, 0x7fff); uint32_t m = 0x80000000u; memcpy(R + 6, &m, 4); ret = 2; }
   else if (x > -0x1451) { sub_CD3BC(e, reinterpret_cast<P>(T), x, a4); memcpy(R, T, 12); }
   else ret = 1;
  }
 }
 if (ret == 1 || ret == 4 || (ret == 0 && nd == 0)) memset(R, 0, 12), ret = ret ? ret : 0;
 memcpy(reinterpret_cast<void*>(out), R, 2); memcpy(reinterpret_cast<uint8_t*>(out) + 2, R + 2, 4);
 memcpy(reinterpret_cast<uint8_t*>(out) + 6, R + 6, 4);
 st16(reinterpret_cast<uint8_t*>(out) + 10, ld16(R + 10) | sign);
 return ret;
}

// 1.13c D2Client+0xCCD43  ext -> decimal digits (_fltout)  out: i16 decpt, u8 sign@2, u8 len@3, digits@4
uint32_t sub_CCD43(const Env& e, uint32_t lo, uint32_t hi, uint16_t se, int nd, uint8_t flag, P outp) {
 uint8_t* o = reinterpret_cast<uint8_t*>(outp); uint32_t v = se & 0x7fff;
 uint8_t K[12]; memset(K, 0xcc, 10); K[10] = 0xfb; K[11] = 0x3f;
 o[2] = (se & 0x8000) ? '-' : ' ';
 auto zero = [&]() { st16(o, 0); o[2] = ' '; o[3] = 1; o[5] = 0; };
 if ((uint16_t)v == 0 && hi == 0 && lo == 0) { o[4] = '0'; zero(); return 1; }
 if ((uint16_t)v == 0x7fff) {
  st16(o, 1); const char* s;
  if ((hi == 0x80000000u && lo == 0) || (hi & 0x40000000u)) {
   if (!(se & 0x8000) || hi != 0xc0000000u) { if (hi != 0x80000000u || lo != 0) { s = "1#QNAN"; o[3] = 6; } else { s = "1#INF"; o[3] = 5; } }
   else if (lo != 0) { s = "1#QNAN"; o[3] = 6; } else { s = "1#IND"; o[3] = 5; }
  } else { s = "1#SNAN"; o[3] = 6; }
  memcpy(o + 4, s, strlen(s) + 1); return 0;
 }
 uint8_t T[12] = {0}; st16(T + 10, (uint16_t)v); memcpy(T + 2, &lo, 4); memcpy(T + 6, &hi, 4);
 int16_t dec = (int16_t)(((v >> 8) + (hi >> 24) * 2) * 0x4d - 0x134312f4u + v * 0x4d10 >> 16);
 sub_CD3BC(e, reinterpret_cast<P>(T), -(int32_t)dec, 1);
 if (ld16(T + 10) > 0x3ffe) { dec++; sub_CD186(reinterpret_cast<P>(T), reinterpret_cast<P>(K)); }
 st16(o, (uint16_t)dec);
 if (flag & 1) { nd += dec; if (nd < 1) { o[4] = '0'; zero(); return 1; } }
 if (nd > 0x15) nd = 0x15;
 int32_t ex = (int32_t)ld16(T + 10) - 0x3ffe; T[10] = T[11] = 0;
 for (int i = 0; i < 8; i++) shl12(W(reinterpret_cast<P>(T)));
 if (ex < 0) for (uint32_t i = (uint32_t)-ex & 0xff; i; i--) sub_CC7EE(reinterpret_cast<P>(T));
 uint8_t* p = o + 4;
 for (int n = nd + 1; n > 0; n--) {
  uint32_t* t = W(reinterpret_cast<P>(T)); uint32_t c[3] = {t[0], t[1], t[2]};
  shl12(t); shl12(t); add12(t, c); shl12(t);
  *p++ = (uint8_t)(T[11] + '0'); T[11] = 0;
 }
 uint8_t *q = p - 1, *b = o + 4;
 if (*q < '5') { while (q >= b && *q == '0') q--; if (q < b) { *b = '0'; zero(); return 1; } }
 else { while (q >= b && *q == '9') *q-- = '0'; if (q < b) { q++; st16(o, (uint16_t)(ld16(o) + 1)); } (*q)++; }
 o[3] = (uint8_t)(q - o - 3); o[(uint8_t)o[3] + 4] = 0;
 return 1;
}

// 1.13c D2Client+0xCCFDF  _siglookup: row with signum==sig in the action table, 0 if absent
P sub_CCFDF(const Env& e, int sig) {
 for (P r = e.sigTable; r < e.sigTable + e.sigCount * 12; r += 12) if ((int)e.rd32(r + 4) == sig) return r;
 return 0;
}

// 1.13c D2Client+0xCD00D  _raise. approximate: no per-thread ptd/FPE sub-code save, no lock
int sub_CD00D(const Env& e, int sig) {
 P slot;
 if (sig == 2) slot = e.sigIntSlot; else if (sig == 15) slot = e.sigTermSlot; else if (sig == 0x15) slot = e.sigBreakSlot;
 else if (sig == 0x16) slot = e.sigAbortSlot;
 else if (sig == 4 || sig == 8 || sig == 0xb) { P r = sub_CCFDF(e, sig); if (!r) return -1; slot = r + 8; }
 else return -1;
 P h = e.rd32(slot);
 if (h == 1) return 0;
 if (h == 0) { e.exitFn(3); return 0; }
 e.wr32(slot, 0);
 if (sig == 8) e.callHandler(h, 8, 0x8c); else e.callHandler(h, sig, 0);
 return 0;
}

// 1.13c D2Client+0xCD55E, +0xCD593, +0xCD5C2  SEH unwind thunks: FUN_6fab1000(*(ebp-0x10))
void sub_CD55E(const Env& e, P frameSlot) { e.f1000(e.rd32(frameSlot)); }
void sub_CD593(const Env& e, P frameSlot) { e.f1000(e.rd32(frameSlot)); }
void sub_CD5C2(const Env& e, P frameSlot) { e.f1000(e.rd32(frameSlot)); }
// 1.13c D2Client+0xCD573  clear bit 0 of DAT_6fbd3700
void sub_CD573(const Env& e) { e.wr32(e.flag3700, e.rd32(e.flag3700) & 0xfffffffeu); }
}
