// src/core/storm/storm_n29_ld12.cpp -- Storm.dll 80-bit "_LDBL12" string<->float helpers and SNet
// wrappers, transcribed from the cached decompiles. A _LDBL12 is 12 bytes: [0..1] pad, [2..9]
// mantissa, [10..11] sign|exponent. Retail data tables, globals and callees are host seams.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t rd32(const uint8_t* p) { uint32_t v; memcpy(&v, p, 4); return v; }
inline void wr32(uint8_t* p, uint32_t v) { memcpy(p, &v, 4); }
inline uint16_t rd16(const uint8_t* p) { uint16_t v; memcpy(&v, p, 2); return v; }
inline void wr16(uint8_t* p, uint16_t v) { memcpy(p, &v, 2); }
inline bool dig(uint8_t c) { return c >= '0' && c <= '9'; }
inline bool dig19(uint8_t c) { return c > '0' && c < ':'; }
// 1.13c Storm+0x408A6 (___add_12), 0x40904 (___shl_12), 0x40932 (___shr_12): 96-bit helpers.
void add12(uint32_t* a, const uint32_t* b) {
  uint64_t s = (uint64_t)a[0] + b[0]; a[0] = (uint32_t)s; uint32_t c = s >> 32;
  if (c) { uint64_t t = (uint64_t)a[1] + 1; a[1] = (uint32_t)t; if (t >> 32) a[2]++; }
  uint64_t t = (uint64_t)a[1] + b[1]; a[1] = (uint32_t)t; if (t >> 32) a[2]++;
  a[2] += b[2];
}
void shl12(uint32_t* a) { uint32_t x = a[0], y = a[1]; a[0] = x << 1; a[1] = y << 1 | x >> 31; a[2] = a[2] << 1 | y >> 31; }
void shr12(uint32_t* a) { uint32_t y = a[1]; a[1] = y >> 1 | a[2] << 31; a[2] >>= 1; a[0] = a[0] >> 1 | y << 31; }
}  // namespace

extern "C" {
// Retail data seams: 0x6FC42140 / 0x6FC422A0 power-of-ten tables (12-byte entries), 0x6FC406B4 decimal point.
struct StormLd12Env { const uint8_t* posPow; const uint8_t* negPow; uint8_t decimalPoint; };

// 1.13c Storm+0x4095F
void Storm_Sub4095F(const char* s, int n, uint32_t* out) {
  int16_t ex = 0x404e; out[0] = out[1] = out[2] = 0;
  for (; n; n--) {
    uint32_t t[3] = {out[0], out[1], out[2]};
    shl12(out); shl12(out); add12(out, t); shl12(out);
    uint32_t d[3] = {(uint32_t)(int32_t)*s++, 0, 0}; add12(out, d);
  }
  if (out[2] == 0) {
    uint32_t u;
    do { ex -= 0x10; u = out[1] >> 16; out[1] = out[0] >> 16 | out[1] << 16; out[0] <<= 16; } while (u == 0);
    out[2] = u;
  }
  while ((out[2] & 0x8000) == 0) { shl12(out); ex--; }
  wr16((uint8_t*)out + 10, (uint16_t)ex);
}

// 1.13c Storm+0x416A8
void Storm_Sub416A8(uint8_t* a, const uint8_t* b) {
  uint8_t acc[12] = {0}; int32_t lost = 0;
  uint16_t eb = rd16(b + 10) & 0x7fff, ea = rd16(a + 10) & 0x7fff;
  uint16_t sign = (rd16(b + 10) ^ rd16(a + 10)) & 0x8000, e = eb + ea, ex;
  bool inf = false;
  if (ea < 0x7fff && eb < 0x7fff && e < 0xbffe) {
    if (e < 0x3fc0) goto zero;
    if (ea == 0) {
      e++; ex = 0;
      if ((rd32(a + 8) & 0x7fffffff) == 0 && rd32(a + 4) == 0 && rd32(a) == 0) { wr16(a + 10, 0); return; }
    }
    if (eb == 0 && (e++, (rd32(b + 8) & 0x7fffffff) == 0 && rd32(b + 4) == 0 && rd32(b) == 0)) {
    zero: wr32(a, 0); wr32(a + 4, 0); wr32(a + 8, 0); return;
    }
    for (int i = 0, cnt = 5; cnt > 0; i++, cnt--)
      for (int j = 0; j < cnt; j++) {
        uint32_t x = rd32(acc + i * 2), m = (uint32_t)rd16(a + (i + j) * 2) * rd16(b + 8 - j * 2);
        uint32_t s = x + m; wr32(acc + i * 2, s);
        if (s < x || s < m) wr16(acc + (i + 2) * 2, rd16(acc + (i + 2) * 2) + 1);
      }
    ex = e + 0xc002;
    bool under = (int16_t)ex < 1;
    if (!under) {
      do { if (acc[11] & 0x80) break; uint32_t* q = (uint32_t*)acc; shl12(q); ex--; } while ((int16_t)ex > 0);
      under = (int16_t)ex < 1;
    }
    if (under) {
      ex--;
      if ((int16_t)ex < 0) {
        uint32_t n = (uint16_t)-ex; ex = 0;
        do { if (acc[0] & 1) lost++; shr12((uint32_t*)acc); } while (--n);
        if (lost) acc[0] |= 1;
      }
    }
    if (rd16(acc) > 0x8000 || ((rd32(acc) & 0x1ffff) == 0x18000)) {
      int k = 2; while (k < 12 && ++acc[k] == 0) k++;
      if (k == 12) { ex++; acc[10] = 0; acc[11] = 0x80; }  // all ones rolled over
    }
    if (ex > 0x7ffe) goto ovf;
    memcpy(a, acc + 2, 10); ex |= sign; wr16(a + 10, ex); return;
  }
ovf: (void)inf;
  wr32(a, 0); wr32(a + 4, 0); wr32(a + 8, (sign ? 0x80000000u : 0) + 0x7fff8000u);
}

// 1.13c Storm+0x418DE
void Storm_Sub418DE(const StormLd12Env& e, uint8_t* a, uint32_t n, int flag) {
  const uint8_t* t = e.posPow;
  if (!n) return;
  if ((int32_t)n < 0) { n = -(int32_t)n; t = e.negPow; }
  if (!flag) wr16(a, 0);
  while (n) {
    uint32_t r = n & 7; n = (int32_t)n >> 3; t += 0x54;
    if (r) {
      const uint8_t* ent = t + r * 12; uint8_t tmp[12];
      if (rd16(ent) > 0x7fff) { memcpy(tmp, ent, 12); wr32(tmp + 2, rd32(tmp + 2) - 1); ent = tmp; }
      Storm_Sub416A8(a, ent);
    }
  }
}

// 1.13c Storm+0x40A47
int Storm_Sub40A47(const StormLd12Env& env, uint8_t* out, const uint8_t** endp, const uint8_t* p,
                   int flag, int bias1, int bias2, int allowExp) {
  int state = 0, sgn = 1, exp10 = 0, adj = 0, ret = 0; uint32_t nd = 0; bool b1 = false, b2 = false, b3 = false;
  uint16_t sbit = 0; const uint8_t* save = p; char buf[32] = {0}; char* w = buf; uint8_t c; const uint8_t* q;
  while ((c = *p) == 0x20 || c == 9 || c == 10 || c == 0xd) p++;
  for (;;) {
    c = *p; q = p + 1;
    switch (state) {
    case 0:
      if (dig19(c)) { state = 3; p = q - 1; continue; }
      if (c == env.decimalPoint) { state = 5; p = q; continue; }
      state = 2;
      if (c == '+') { sbit = 0; p = q; continue; }
      if (c == '-') { sbit = 0x8000; p = q; continue; }
      goto leadzero;
    case 1:
      b1 = true;
      if (dig19(c)) { state = 3; p = q - 1; continue; }
      if (c == env.decimalPoint) goto dot;
      if (c == '+' || c == '-') goto tailsign;
      p = q; if (c != '0') goto expchk; continue;
    case 2:
      if (dig19(c)) { state = 3; p = q - 1; continue; }
      p = save; if (c == env.decimalPoint) { state = 5; p = q; continue; }
      goto leadzero;
    case 3:
      while (dig(c)) { if (nd < 0x19) { nd++; *w++ = c - '0'; } else adj++; c = *q++; }
      if (c != env.decimalPoint) goto afterdig;
    dot: b1 = true; state = 4; p = q; continue;
    case 4:
      b3 = true;
      if (nd == 0) while (c == '0') { adj--; c = *q++; }
      while (dig(c)) { if (nd < 0x19) { nd++; *w++ = c - '0'; adj--; } c = *q++; }
    afterdig:
      if (c == '+' || c == '-') {
      tailsign: b1 = true; state = 0xb; p = q - 1;
      } else {
      expchk: b1 = true;
        if (c < 'D' || (c > 'E' && (c < 'd' || c > 'e'))) { p = q - 1; goto done; }
        state = 6; p = q;
      }
      continue;
    case 5:
      b3 = true; p = save;
      if (dig(c)) { state = 4; p = q - 1; continue; }
      goto done;
    case 6:
      save = p - 1;
      if (c < '1' || c > '9') {
        if (c == '+') { state = 7; p = q; continue; }
        if (c == '-') { sgn = -1; state = 7; p = q; continue; }
        p = save; if (c == '0') { state = 8; p = q; continue; }
        goto done;
      }
      state = 9; p = q - 1; continue;
    case 7:
      if (dig19(c)) { state = 9; p = q - 1; continue; }
      p = save; if (c != '0') goto done;
      state = 8; p = q; continue;
    case 8:
      b2 = true; while (c == '0') c = *q++;
      if (dig19(c)) { state = 9; p = q - 1; continue; }
      p = q - 1; goto done;
    case 9: {
      b2 = true; exp10 = 0;
      while (dig(c)) {
        exp10 = (char)c - '0' + exp10 * 10;
        if (exp10 > 0x1450) { exp10 = 0x1451; break; }
        c = *q++;
      }
      while (dig(c)) c = *q++;
      p = q - 1; goto done;
    }
    case 0xb:
      if (allowExp) {
        save = p;
        if (c == '+') { state = 7; p = q; continue; }
        if (c != '-') goto done;
        sgn = -1; state = 7; p = q; continue;
      }
      state = 10; goto done;
    default:
      p = q; if (state != 10) continue;
      goto done;
    }
  leadzero:
    if (c != '0') goto done;
    state = 1; p = q; continue;
  }
done:;
  uint8_t r[12];
  uint16_t hi; uint32_t m2;
  if (!b1) { ret = 4; goto zero; }
  if (nd > 0x18) { if (buf[23] > 4) buf[23]++; w--; adj++; nd = 0x18; }
  if (nd == 0) goto zero;
  while (*--w == 0) { nd--; adj++; }
  Storm_Sub4095F(buf, nd, (uint32_t*)r);
  if (sgn < 0) exp10 = -exp10;
  {
    uint32_t tot = exp10 + adj;
    if (!b2) tot += bias1;
    if (!b3) tot -= bias2;
    if ((int32_t)tot > 0x1450) { memset(r, 0, 12); wr16(r + 10, 0x7fff); wr32(r + 6, 0x80000000u); ret = 2; goto out; }
    if ((int32_t)tot > -0x1451) { Storm_Sub418DE(env, r, tot, flag); goto out; }
  }
zero1: ret = 1;
zero: memset(r, 0, 12);
out:
  *endp = p; hi = rd16(r + 10) | sbit; m2 = 0; (void)m2;
  memcpy(out, r, 10); wr16(out + 10, hi);
  return ret;
}

// 1.13c Storm+0x40E87
// +3 digit count, +4 digits NUL. Returns 1, or 0 for INF/NAN (text in digits).
int Storm_Sub40E87(const StormLd12Env& env, uint32_t lo, uint32_t mid, uint16_t se, int ndig, uint8_t flags, uint8_t* out) {
  uint32_t ex = se & 0x7fff; uint8_t* dg = out + 4;
  uint8_t tenth[12]; memset(tenth, 0xcc, 10); tenth[10] = 0xfb; tenth[11] = 0x3f;
  out[2] = (se & 0x8000) ? '-' : ' ';
  auto zero = [&]() { out[0] = out[1] = 0; out[2] = ' '; out[3] = 1; out[5] = 0; };
  if (ex == 0 && mid == 0 && lo == 0) { dg[0] = '0'; zero(); return 1; }
  if (ex == 0x7fff) {
    wr16(out, 1); const char* s; int len = 6;
    if ((mid == 0x80000000 && lo == 0) || (mid & 0x40000000)) {
      if (!(se & 0x8000) || mid != 0xc0000000) { if (mid != 0x80000000 || lo != 0) s = "1#QNAN"; else { s = "1#INF"; len = 5; } }
      else { if (lo != 0) s = "1#QNAN"; else { s = "1#IND"; len = 5; } }
    } else s = "1#SNAN";
    strcpy((char*)dg, s); out[3] = len; return 0;
  }
  uint8_t w[12] = {0}; wr32(w + 2, lo); wr32(w + 6, mid);
  int16_t dp = (int16_t)((((ex >> 8) + (mid >> 24) * 2) * 0x4d + (uint32_t)-0x134312f4 + ex * 0x4d10) >> 16);
  w[10] = ex; w[11] = ex >> 8;
  Storm_Sub418DE(env, w, -(int32_t)dp, 1);
  if (rd16(w + 10) > 0x3ffe) { dp++; Storm_Sub416A8(w, tenth); }
  wr16(out, dp);
  if ((flags & 1) && (ndig += dp, ndig < 1)) { dg[0] = '0'; zero(); return 1; }
  if (ndig > 0x15) ndig = 0x15;
  int sh = (int)rd16(w + 10) - 0x3ffe;
  w[10] = w[11] = 0;
  for (int i = 8; i; i--) shl12((uint32_t*)w);
  if (sh < 0) for (uint32_t k = -sh & 0xff; k; k--) shr12((uint32_t*)w);
  uint8_t* end = dg;
  for (int n = ndig + 1; n > 0; n--) {
    uint8_t cp[12]; memcpy(cp, w, 12);
    shl12((uint32_t*)w); shl12((uint32_t*)w); add12((uint32_t*)w, (uint32_t*)cp); shl12((uint32_t*)w);
    *end++ = w[11] + '0'; w[11] = 0;
  }
  uint8_t* q = end - 2;
  if (end[-1] < '5') {
    while (dg <= q && *q == '0') q--;
    if (q < dg) { *dg = '0'; out[0] = out[1] = 0; out[2] = ' '; out[3] = 1; out[5] = 0; return 1; }
  } else {
    while (dg <= q && *q == '9') *q-- = '0';
    if (q < dg) { q++; wr16(out, rd16(out) + 1); }
    (*q)++;
  }
  out[3] = (uint8_t)((q - out) - 3); out[(q - out) + 1] = 0;
  return 1;
}
}  // extern "C"

// ---- SNet wrappers (ordinals 111-118). Retail globals and callees are seams in StormNetEnv. -------
extern "C" {
struct StormNetProvider {            // table at DAT_6FC45834 (retail offsets in the comments)
  int (*f14)(uint32_t, const char*, const char*, uint32_t*);               // +0x14
  void (*f18)(uint32_t, uint32_t*, uint64_t*, uint32_t*);                  // +0x18
  int (*f20)(uint32_t, void*, uint32_t*, uint32_t*, uint32_t*);            // +0x20
  void (*f50)(char*, int, char*, int);                                     // +0x50
};
struct StormNetEnv {
  uint32_t lastError;                        // DAT_6FC45388
  void (*setLastError)(uint32_t);            // SetLastError
  void (*lock)(); void (*unlock)();          // DAT_6FC45A24 critical section
  uint32_t slots[16][4];                     // DAT_6FC41A38, 16 bytes per entry
  StormNetProvider* provider;                // DAT_6FC45834
  uint8_t* providerInfo;                     // DAT_6FC45840 (+0x214: 36 bytes)
  int32_t cur;                               // DAT_6FC41B4C
  int32_t base;                              // DAT_6FC45830
  uint32_t gA, gB, gC, g4534c, g45174, g41b3c; // DAT_6FC45340, 6FC4516C, 6FC45354, 6FC4534C, 6FC45174, 6FC41B3C
  void (*nowFileTime)(uint64_t*);            // GetSystemTime + SystemTimeToFileTime
  uint8_t* (*findPlayer)(int32_t);           // 0xFEB0
  int (*parseAddr)(const char*, const char*, const char*, uint32_t* b0, uint32_t* a50, uint32_t* a0); // 0x11F10 (b0[4], a50[20], a0[5])
  void (*msgInit)(uint32_t tag, int op);                                   // 0x26020
  void (*msgReg)(uint32_t tag, int a, int id, int z, uint32_t handlerRva); // 0x26290
  int (*f61e0)(); int (*f6e70)(uint32_t* blk, uint32_t* b0, uint32_t* a50, uint32_t* a0, int z); // 0x161E0, 0x16E70
  void (*f3a90)(int); void (*f1690)(); void (*f5110)(); uint8_t* (*f26f0)(); // 0x13A90 0x11690 0x5110 0x126F0
  uint32_t* (*f2f40)(uint8_t, int8_t, const void*, uint32_t); void (*f1d90)(uint8_t*, uint32_t); // 0x12F40 0x11D90
  void (*f1470)(uint8_t*, int); int (*f49e0)(int, uint32_t); uint8_t* (*f2690)(); void (*f25c0)(uint32_t, const char*); void (*f1240)(); // 0x11470 0x149E0 0x12690 0x125C0 0x11240
};
static void nerr(StormNetEnv& e, uint32_t c) { e.lastError = c; e.setLastError(c); }
static uint32_t sncpy(char* d, const char* s, int n) { uint32_t l = 0; while (l + 1 < (uint32_t)n && s[l]) { d[l] = s[l]; l++; } d[l] = 0; return l; }  // 0x3B080

// 1.13c Storm+0x103F0
int Storm_Sub103F0(StormNetEnv& e, uint32_t i, uint32_t* a, uint32_t* b, uint32_t* c, uint64_t* ft, uint32_t* res) {
  if (i > 0xf || i == 0 || !a) { nerr(e, 0x57); return 0; }
  e.lock();
  *a = e.slots[i][0]; if (b) *b = e.slots[i][1]; if (c) *c = e.slots[i][2];
  if (ft) e.nowFileTime(ft);
  if (res) { res[0] = 10000000; res[1] = 0; }
  if (e.slots[i][3] && e.provider) {
    uint64_t lft; uint32_t lres[2];
    e.provider->f18(i, a, ft ? ft : &lft, res ? res : lres);
  }
  e.unlock(); return 1;
}

// 1.13c Storm+0x102E0
int Storm_Sub102E0(StormNetEnv& e, int32_t player, uint32_t* o) {
  if (!o || o[0] != 0x24) { nerr(e, 0x57); return 0; }
  e.lock();
  for (int k = 1; k <= 8; k++) o[k] = 0;
  uint32_t err;
  if (!e.provider || !e.providerInfo) err = 0x4b4;
  else if (e.cur == -1) err = 0x85100070;
  else {
    uint8_t* x = e.findPlayer(player - e.base);
    if (x) {
      memcpy(o, e.providerInfo + 0x214, 36);
      if (rd32(x + 0x128)) o[6] = rd32(x + 0x128);
      e.unlock(); return 1;
    }
    err = 0x8510006a;
  }
  nerr(e, err); e.unlock(); return 0;
}

// 1.13c Storm+0x10200
int Storm_Sub10200(StormNetEnv& e, int32_t player, char* dst, int n) {
  if (!dst || !n) { nerr(e, 0x57); return 0; }
  e.lock(); *dst = 0;
  uint32_t err;
  if (!e.provider) err = 0x4b4;
  else if (e.cur == -1) err = 0x85100070;
  else {
    uint8_t* x = e.findPlayer(player - e.base);
    if (x && !(x[0x118] & 4)) { sncpy(dst, (const char*)(x + 8), n); e.unlock(); return 1; }
    err = 0x8510006a;
  }
  nerr(e, err); e.unlock(); return 0;
}

// 1.13c Storm+0x10140
int Storm_Sub10140(StormNetEnv& e, uint32_t* o) {
  if (!o || o[0] != 0x24) { nerr(e, 0x57); return 0; }
  e.lock();
  for (int k = 1; k <= 8; k++) o[k] = 0;
  if (e.provider && e.providerInfo) { memcpy(o, e.providerInfo + 0x214, 36); e.unlock(); return 1; }
  nerr(e, 0x4b4); e.unlock(); return 0;
}

// 1.13c Storm+0x12170
int Storm_Sub12170(StormNetEnv& e, uint32_t p1, const char* s3, const char* s4, const char* s5) {
  e.lock();
  if (!e.provider) { nerr(e, 0x4b4); e.unlock(); return 0; }
  uint32_t b0[4], a50[20], a0[5]; uint8_t blk[60];
  if (!e.parseAddr(s3, s4, s5, b0, a50, a0)) { nerr(e, 0x57); e.unlock(); return 0; }
  int r = e.provider->f20(p1, blk, b0, a50, a0);
  e.unlock(); return r ? 1 : 0;
}

// 1.13c Storm+0x170B0
int Storm_Sub170B0(StormNetEnv& e, const char* s3, const char* s4, const char* s5) {
  e.lock();
  uint32_t b0[4], a50[20], a0[5], blk[16];
  if (!e.parseAddr(s3, s4, s5, b0, a50, a0)) { nerr(e, 0x57); e.unlock(); return 0; }
  const uint32_t T = 0x534e4554;
  e.msgInit(T, 1); e.msgInit(T, 2);
  static const struct { int id; uint32_t h; } reg[] = {{2,0x14ef0},{0xc,0x10980},{0xe,0xfe90},{0xd,0x10f30},{0xf,0xfe70},{4,0x14ec0},{5,0x12f00},{6,0x12d40},{7,0x14a80},{8,0x12c90},{9,0x12bf0},{10,0xfe60},{0xb,0x12bb0}};
  for (auto& r : reg) e.msgReg(T, 2, r.id, 0, r.h);
  if (!e.f61e0()) { nerr(e, 0xa4); e.unlock(); return 0; }
  if (!e.f6e70(blk, b0, a50, a0, 0)) { e.unlock(); return 0; }
  e.gC = blk[10]; e.gA = blk[3]; e.gB = blk[4];
  e.unlock(); return 1;
}

// 1.13c Storm+0x15C10
int Storm_Sub15C10(StormNetEnv& e, uint32_t p1, const char* s2, const char* s3, const char* s4, const char* s5, int32_t* out) {
  static const char empty[] = "";
  if (!out) { nerr(e, 0x57); return 0; }
  *out = e.base - 1;
  e.lock();
  if (!e.provider) { nerr(e, 0x4b4); e.unlock(); return 0; }
  if (!s2) s2 = empty; if (!s3) s3 = empty; if (!s4) s4 = empty; if (!s5) s5 = empty;
  char l1[0x80] = {0}, l2[0x80] = {0};
  if (!*s4 && e.provider->f50) { e.provider->f50(l1, 0x80, l2, 0x80); s4 = l1; s5 = l2; }
  if (!*s4) { nerr(e, 0x57); return 0; }  // retail returns here still holding the lock
  if (e.cur != 0xff) e.f3a90(2);
  e.f1690(); e.f5110();
  uint32_t buf[0x51]; memset(buf + 2, 0, 0x4f * 4);
  buf[79] = e.gA; buf[80] = e.gB;
  uint32_t err;
  if (!e.provider->f14(p1, s2, s3, buf + 2)) { e.unlock(); return 0; }
  if (buf[79] == e.gA && buf[80] == e.gB) {
    e.g4534c = 1;
    uint8_t* s = e.f26f0();
    if (!s) { e.g4534c = 0; e.f3a90(1); err = 8; }
    else {
      wr32(s + 0x204, 1); wr16(s + 0x1d4, 1); wr16(s + 0x1de, 1); wr16(s + 0x1cc, 1);
      buf[0] = 1;
      uint32_t* r = e.f2f40(0, 1, buf, 4);
      if (r) { e.f1d90(s, r[3]); e.f1d90(s, r[3]); }
      e.f1470(s, 0);
      buf[0] = 2;
      if (e.f49e0(0, e.g41b3c)) {
        char nb[0x300]; uint32_t l1n = sncpy(nb, s4, 0x80);
        uint32_t l2n = sncpy(nb + l1n + 1, s5, 0x80);
        uint32_t l3n = sncpy(nb + l1n + l2n + 2, s3, 0x80);
        e.f26f0();
        e.f2f40(0, 7, nb, l1n + l2n + l3n + 3);
        buf[0] = 9; buf[1] = 10;
        if (e.f49e0(0, e.g41b3c)) {
          if (e.cur != 0xff) {
            e.g45174 = buf[11];
            uint8_t* t = e.f2690();
            if (t) {
              t[0x212] = (uint8_t)e.cur; sncpy((char*)t + 8, s4, 0x80); sncpy((char*)t + 0x88, s5, 0x80);
              e.f25c0((uint8_t)e.cur, s4);
            }
            *out = (uint8_t)e.cur + e.base;
            e.unlock(); e.f1240(); e.lock();
            wr32(s + 0x204, 0); e.g4534c = 0;
            if (e.cur == 0xff) { nerr(e, 0x85100069); e.unlock(); return 0; }
            e.unlock(); return 1;
          }
          wr32(s + 0x204, 0); e.g4534c = 0; e.f3a90(1); err = 0x85100067; nerr(e, err); e.unlock(); return 0;
        }
        wr32(s + 0x204, 0);
      }
      e.g4534c = 0; e.f3a90(1); err = 0x4d0;
    }
  } else err = 0x85100079;
  nerr(e, err); e.unlock(); return 0;
}
}  // extern "C"
