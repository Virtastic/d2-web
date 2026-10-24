// D2sound CRT helpers (VS2003 float runtime). Host seams are passed as args.
#include <cstdint>
#include <cstring>
typedef uint32_t u32; typedef uint16_t u16; typedef int32_t i32;

// 1.13c D2sound+0xC9E1
void ___dtold(u32* p1, u32* p2) {
  u16 hi = *(u16*)((char*)p2 + 6);
  u32 top = 0x80000000u;
  u16 e = (hi >> 4) & 0x7ff;
  u32 lo = *p2;
  if (e == 0) {
    if ((p2[1] & 0xfffff) == 0 && lo == 0) { p1[1] = 0; p1[0] = 0; *(u16*)(p1 + 2) = 0; return; }
    e = e + 0x3c01; top = 0;
  } else if (e == 0x7ff) e = 0x7fff;
  else e = e + 0x3c00;
  p1[1] = lo >> 21 | (p2[1] & 0xfffff) << 11 | top;
  p1[0] = lo << 11;
  while (top == 0) {
    u32 t = p1[1]; e--;
    p1[1] = t << 1 | p1[0] >> 31; p1[0] <<= 1; top = t << 1 & 0x80000000u;
  }
  *(u16*)(p1 + 2) = (hi & 0x8000) | e;
}

// 1.13c D2sound+0xCA9B
// seams: d7e7 = FUN_D7E7, c40c0 = FUN_40C0 (copy string)
i32* FUN_CA9B(u32 d_lo, u32 d_hi, i32* out, u32* str,
              i32 (*d7e7)(u32, u32, u16, int, int, short*),
              void (*c40c0)(u32*, const void*)) {
  u32 dbl[2] = {d_lo, d_hi};
  struct { short sign; char dec; char pad; u32 digits[6]; } r;
  u32 ld[3];
  ___dtold(ld, dbl);
  i32 n = d7e7(ld[0], ld[1], (u16)ld[2], 0x11, 0, &r.sign);
  out[2] = n; out[0] = (i32)r.dec; out[1] = (i32)r.sign;
  c40c0(str, r.digits);
  out[3] = (i32)(intptr_t)str;
  return out;
}

// 1.13c D2sound+0xCB1C
// bx = x87 control word (retail reads it from BX)
u32 __abstract_cw(u16 bx) {
  u32 r = 0;
  if (bx & 1) r = 0x10;
  if (bx & 4) r |= 8;
  if (bx & 8) r |= 4;
  if (bx & 0x10) r |= 2;
  if (bx & 0x20) r |= 1;
  if (bx & 2) r |= 0x80000;
  u16 rc = bx & 0xc00;
  if (rc == 0x400) r |= 0x100; else if (rc == 0x800) r |= 0x200; else if (rc == 0xc00) r |= 0x300;
  if ((bx & 0x300) == 0) r |= 0x20000; else if ((bx & 0x300) == 0x200) r |= 0x10000;
  if (bx & 0x1000) r |= 0x40000;
  return r;
}

// 1.13c D2sound+0xCBAE
u32 __hw_cw(u32 ebx) {
  u32 r = (ebx & 0x10) != 0;
  if (ebx & 8) r |= 4;
  if (ebx & 4) r |= 8;
  if (ebx & 2) r |= 0x10;
  if (ebx & 1) r |= 0x20;
  if (ebx & 0x80000) r |= 2;
  u32 rc = ebx & 0x300;
  if (rc == 0x100) r |= 0x400; else if (rc == 0x200) r |= 0x800; else if (rc == 0x300) r |= 0xc00;
  if ((ebx & 0x30000) == 0) r |= 0x300; else if ((ebx & 0x30000) == 0x10000) r |= 0x200;
  if (ebx & 0x40000) r |= 0x1000;
  return r;
}

// 1.13c D2sound+0xCC3C
// cw = current x87 control word; hwOut receives the __hw_cw result (retail applies it with fldcw)
u32 __control87(u32 nv, u32 mask, u16 cw, u32 ebx, u32* hwOut) {
  u32 r = __abstract_cw(cw);
  u32 hw = __hw_cw(ebx);
  if (hwOut) *hwOut = hw;
  return r & ~mask | nv & mask;
}

// 1.13c D2sound+0xCC6E
u32 __controlfp(u32 nv, u32 mask, u16 cw, u32 ebx, u32* hwOut) {
  return __control87(nv, mask & 0xfff7ffffu, cw, ebx, hwOut);
}

// 1.13c D2sound+0xD1E5
u32 ___addl(u32 a, u32 b, u32* out) {
  u32 s = a + b;
  u32 c = (s < a || s < b) ? 1 : 0;
  *out = s;
  return c;
}

// 1.13c D2sound+0xD206
void ___add_12(u32* a, u32* b) {
  if (___addl(a[0], b[0], a)) {
    if (___addl(a[1], 1, a + 1)) a[2]++;
  }
  if (___addl(a[1], b[1], a + 1)) a[2]++;
  ___addl(a[2], b[2], a + 2);
}

// 1.13c D2sound+0xD264
void ___shl_12(u32* a) {
  u32 l = a[0], m = a[1];
  a[0] = l * 2; a[1] = m * 2 | l >> 31; a[2] = a[2] << 1 | m >> 31;
}

// 1.13c D2sound+0xD292
void ___shr_12(u32* a) {
  u32 m = a[1];
  a[1] = m >> 1 | a[2] << 31; a[2] >>= 1; a[0] = a[0] >> 1 | m << 31;
}

// 1.13c D2sound+0xD2BF
void FUN_D2BF(const char* s, i32 n, u32* o) {
  int16_t e = 0x404e;
  o[0] = o[1] = o[2] = 0;
  for (; n; n--, s++) {
    u32 t[3] = {o[0], o[1], o[2]};
    ___shl_12(o); ___shl_12(o); ___add_12(o, t); ___shl_12(o);
    t[0] = (u32)(int32_t)*s; t[1] = 0; t[2] = 0;
    ___add_12(o, t);
  }
  if (o[2] == 0) {
    u32 u;
    do { e -= 0x10; u = o[1] >> 16; o[1] = o[0] >> 16 | o[1] << 16; o[0] <<= 16; } while (u == 0);
    o[2] = u;
  }
  while (!(o[2] & 0x8000)) { ___shl_12(o); e--; }
  *(int16_t*)((char*)o + 10) = e;
}

// 1.13c D2sound+0xE23E
// seams: pos/neg = power-table bases (retail 0x6f9c5cd8 / 0x6f9c5e38), mul = FUN_E008
void FUN_E23E(i32* p1, u32 n, i32 p3, const char* pos, const char* neg,
              void (*mul)(i32*, i32*)) {
  const char* t = pos;
  if (!n) return;
  if ((i32)n < 0) { n = -(i32)n; t = neg; }
  if (p3 == 0) *(int16_t*)p1 = 0;
  while (n) {
    u32 k = n & 7; n = (i32)n >> 3;
    t += 0x54;
    if (k) {
      const u16* e = (const u16*)(t + k * 0xc);
      u16 tmp[6];
      if (*e > 0x7fff) {
        memcpy(tmp, e, 12);
        u32 m; memcpy(&m, (char*)tmp + 2, 4); m -= 1; memcpy((char*)tmp + 2, &m, 4);
        e = tmp;
      }
      mul(p1, (i32*)e);
    }
  }
}
