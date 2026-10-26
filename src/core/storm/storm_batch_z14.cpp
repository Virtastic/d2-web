#include <cstdint>
// Retail callees/globals as seams (z14 prefix keeps names unique).
extern uint32_t z14_lastErr;                      // DAT_6fc45388
void z14_SetLastError(uint32_t);                  // kernel32
int z14_Sub23EA0(uint32_t*, uint32_t*, uint8_t*, uint32_t, uint32_t*, uint32_t*, uint32_t*);
uint8_t* z14_Sub0C580(uint32_t, const char*, int, int);
void z14_Sub0B960(void*, const char*, int);
void z14_Sub27C10(uint8_t*, uint32_t, uint32_t, int, uint32_t*, void*, uint32_t*);
void z14_Sub2B080(void*, const void*, uint32_t);
void z14_Strupr(char*);
void z14_Strlwr(char*);
void z14_Sub01120(uint32_t* obj);                 // ctor
void z14_Sub011A0(uint32_t* obj);                 // dtor
int z14_Sub291E0(uint32_t* obj, const void* key, uint32_t* out);
extern uint32_t z14_vtbl36848;                    // PTR_FUN_6fc36848 (32-bit link)
extern const uint32_t z14_tab35458[];             // pairs (lo,hi), stride 8
long double z14_Pow(int base, int e);             // Storm+0x1000
int64_t z14_Sub2C908();                           // x87 -> int64 after Storm+0x1000
extern const double z14_d37af8, z14_d37b10, z14_d37b08;
extern const double z14_tab42918[];
uint32_t z14_Atoi(char*);                         // Storm+0x3AD50
uint32_t z14_vsprintf(char*, const char*, void*);
uint32_t z14_vsnprintf(char*, uint32_t, const char*, void*);
char* z14_Sub3B280(char*, char);
char* z14_Sub3B2D0(char*, char);

// 1.13c Storm+0x37D50
uint32_t Storm_Sub37D50_z14(uint32_t* name, uint32_t* rect, void* dst, uint32_t* outSize) {
  if (outSize) *outSize = 0;
  if ((char)*name == 0 || !name || !outSize) { z14_lastErr = 0x57; z14_SetLastError(0x57); return 0; }
  uint32_t w = 0, h = 0;
  if (!z14_Sub23EA0(name, 0, 0, 0, &w, &h, 0)) return 0;
  uint32_t n = h * w;
  uint8_t* buf = z14_Sub0C580(n, "..\\3rdParty\\STORM\\SOURCE\\STRANS.CPP", 0x4d9, 0);
  if (!z14_Sub23EA0(name, 0, buf, n, 0, 0, 0)) return 0;
  z14_Sub27C10(buf, w, h, 8, rect, dst, outSize);
  z14_Sub0B960(buf, "..\\3rdParty\\STORM\\SOURCE\\STRANS.CPP", 0x4e7);
  return 1;
}
// 1.13c Storm+0x37E30: inclusive rect -> exclusive (right/bottom +1), then 37D50.
void Storm_Sub37E30_z14(uint32_t* name, uint32_t* r, void* dst, uint32_t* outSize) {
  uint32_t t[4]; uint32_t* p = 0;
  if (r) { t[0] = r[0]; t[1] = r[1]; t[2] = r[2] + 1; t[3] = r[3] + 1; p = t; }
  Storm_Sub37D50_z14(name, p, dst, outSize);
}
// 1.13c Storm+0x3A200
uint32_t Storm_Sub3A200_z14(uint32_t* name, uint8_t* out, uint32_t cap) {
  if ((char)*name == 0 || !name || !out || cap == 0) { z14_lastErr = 0x57; z14_SetLastError(0x57); return 0; }
  uint32_t key[64]; uint32_t obj[8];
  z14_Sub2B080(key, name, 0x100);
  z14_Strupr((char*)key);
  z14_Sub01120(obj);
  obj[0] = z14_vtbl36848;
  if (!z14_Sub291E0(obj, key, 0)) { *out = 0; z14_Sub011A0(obj); return 0; }
  uint32_t n = obj[2] + 1;
  if (cap < n) n = cap;
  z14_Sub2B080(out, (const void*)(uintptr_t)obj[1], n);
  z14_Sub011A0(obj);
  return 1;
}
// 1.13c Storm+0x3A850
void Storm_Sub3A850_z14(char* s) { z14_Strlwr(s); }
// 1.13c Storm+0x3AA90: 64-bit name hash (seed 0 -> 0x7fed7fed:0x7fed7fed); flags&1 skips upper/slash folding.
uint64_t Storm_Sub3AA90_z14(const uint8_t* s, uint8_t flags, uint32_t lo, uint32_t hi) {
  if (!s) { z14_lastErr = 0x57; z14_SetLastError(0x57); return 0; }
  uint64_t v = ((uint64_t)hi << 32) | lo, k = 0xEEEEEEEEEEEEEEEEull;
  if (!lo && !hi) v = 0x7fed7fed7fed7fedull;
  for (uint32_t c; (c = *s) != 0; ++s) {
    if (!(flags & 1)) { if (c > 0x60 && c < 0x7b) c -= 0x20; if (c == 0x2f) c = 0x5c; }
    uint64_t a = z14_tab35458[(c >> 4) * 2] | ((uint64_t)z14_tab35458[(c >> 4) * 2 + 1] << 32);
    uint64_t b = z14_tab35458[(c & 15) * 2] | ((uint64_t)z14_tab35458[(c & 15) * 2 + 1] << 32);
    uint64_t nv = (a - b) ^ (k + v);
    k = k * 0x21 + c + nv + 3;
    v = nv;
  }
  return v ? v : 1;
}
// 1.13c Storm+0x3ACF0: unsigned decimal prefix parse.
uint32_t Storm_Sub3ACF0_z14(const char* s) {
  if (!s) { z14_lastErr = 0x57; z14_SetLastError(0x57); return 0; }
  uint32_t v = (uint32_t)(int)*s - 0x30;
  if (v >= 10) return 0;
  for (uint32_t d; (d = (uint32_t)(int)*++s - 0x30) < 10;) v = d + v * 10;
  return v;
}
// 1.13c Storm+0x3AD50: signed decimal prefix parse.
uint32_t Storm_Sub3AD50_z14(const char* s) {
  if (!s) { z14_lastErr = 0x57; z14_SetLastError(0x57); return 0; }
  char neg = *s; if (neg == '-') ++s;
  uint32_t v = (uint32_t)(int)*s - 0x30;
  if (v < 10) { for (uint32_t d; (d = (uint32_t)(int)*++s - 0x30) < 10;) v = d + v * 10; } else v = 0;
  return neg == '-' ? (uint32_t)-(int32_t)v : v;
}
// 1.13c Storm+0x3B560 (guessed: Sub2C908 converts the x87 pow(10,n) left by Storm+0x1000 to int64)
uint64_t Storm_Sub3B560_z14(const char* s) {
  if (!s) { z14_lastErr = 0x57; z14_SetLastError(0x57); return 0; }
  char neg = *s; if (neg == '-') ++s;
  uint32_t v = (uint32_t)(int)*s - 0x30; int64_t acc = 0; uint64_t r;
  if (v < 10) {
    const char* p = s + 1; uint32_t d = (uint32_t)(int)s[1] - 0x30;
    if (d < 10) {
      do {
        v = d + v * 10; ++p;
        if (v > 0x19999998) { z14_Pow(10, (int)(p - s)); acc = z14_Sub2C908() * acc + v; v = 0; s = p; }
        d = (uint32_t)(int)*p - 0x30;
      } while (d < 10);
      if (acc) { z14_Pow(10, (int)(p - s)); r = (uint64_t)(z14_Sub2C908() * acc + v); goto done; }
    }
  } else v = 0;
  r = v;
done:
  return neg == '-' ? (uint64_t)-(int64_t)r : r;
}
// Shared body of 0x3B660 / 0x3B830 (identical code; differ only in the null-arg return).
static long double z14_StrToFloat(const char* s) {
  char neg = *s; if (neg == '-') ++s;
  uint32_t v = (uint32_t)(int)*s - 0x30; double acc = z14_d37b10; const char* p; long double f;
  auto u = [](uint32_t x) { long double r = (int32_t)x; if ((int32_t)x < 0) r += z14_d37b08; return r; };
  bool plain = true;
  if (v < 10) {
    p = s + 1; uint32_t d = (uint32_t)(int)s[1] - 0x30;
    if (d < 10) {
      do {
        v = d + v * 10; ++p;
        if (v > 0x19999998) { long double pw = z14_Pow(10, (int)(p - s)); acc = (double)(u(v) + pw * acc); v = 0; s = p; }
        d = (uint32_t)(int)*p - 0x30;
      } while (d < 10);
      if (acc != z14_d37b10) { long double pw = z14_Pow(10, (int)(p - s)); f = u(v) + pw * acc; plain = false; }
    }
  } else { v = 0; p = s; }
  if (plain) f = u(v);
  acc = (double)f;
  if (*p == '.') {
    ++p; uint32_t d = (uint32_t)(int)*p - 0x30;
    if (d < 10) {
      int idx = 0; uint32_t e = 0xffffffff;
      do {
        ++p;
        if ((int32_t)e < -0x14) f = u(d) * z14_Pow(10, (int)e) + (long double)acc;
        else f += (long double)z14_tab42918[idx + d];  // sic: retail adds to f, not acc
        acc = (double)f; idx += 10; d = (uint32_t)(int)*p - 0x30; --e;
      } while (d < 10);
    }
  }
  if (*p == 'e' || *p == 'E') {
    const char* q = p + 1; if (p[1] == '+') q = p + 2;
    f = z14_Pow(10, (int)z14_Atoi((char*)q)) * (long double)acc;
  }
  return neg == '-' ? -f : f;
}
// 1.13c Storm+0x3B660
long double Storm_Sub3B660_z14(const char* s) {
  if (!s) { z14_lastErr = 0x57; z14_SetLastError(0x57); return z14_d37af8; }
  return z14_StrToFloat(s);
}
// 1.13c Storm+0x3B830
long double Storm_Sub3B830_z14(const char* s) {
  if (!s) { z14_lastErr = 0x57; z14_SetLastError(0x57); return z14_d37b10; }
  return z14_StrToFloat(s);
}
// 1.13c Storm+0x3BA00: bounded vsprintf; cap 0x7fffffff = unbounded.
uint32_t Storm_Sub3BA00_z14(char* dst, uint32_t cap, const char* fmt, void* ap) {
  if (!dst || !fmt) { z14_lastErr = 0x57; z14_SetLastError(0x57); return 0; }
  if (cap == 0) return 0;
  if (cap == 0x7fffffff) return z14_vsprintf(dst, fmt, ap);
  uint32_t n = z14_vsnprintf(dst, cap, fmt, ap);
  if (cap <= n) { dst[cap - 1] = 0; return cap - 1; }
  return n;
}
// 1.13c Storm+0x3BB00: strrchr (flag != 0) / strchr (flag == 0).
char* Storm_Sub3BB00_z14(char* s, char c, int last) {
  if (!s) { z14_lastErr = 0x57; z14_SetLastError(0x57); return 0; }
  return last ? z14_Sub3B280(s, c) : z14_Sub3B2D0(s, c);
}
