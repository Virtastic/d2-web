// src/core/d2ddraw/d2ddraw_batch_b144.cpp -- UNMAPPED 1.13c D2DDraw.dll rows (alpha lane, no spec/vector).
// Decompiles: build-wasm/ghidra-d2ddraw/decomp/0x{51D0,560C,5B74,6E20,6FF0,85A0,9779,9796,9844,98B4,9950,99B7}.c
// Register-passed inputs the decompiler lost are explicit parameters (marked "reg:").
#include <cstdint>
#include <cstdlib>
#include <cstring>
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif
#define W __attribute__((weak))

typedef void* (*vfn)(...);
static inline vfn slot(void* o, int off) { return (*(vfn**)o)[off / 4]; }

struct STRFLT { int32_t sign; int32_t decpt; int32_t flag; char* mant; };  // retail layout, mant at +0xc

extern "C" {
void _exit(int);
W int32_t DAT_6f8cf2a4, DAT_6f8cf264, DAT_6f8cf274;
W void (*DAT_6f8cf2a8)(void);
W uint8_t DAT_6f8ce7e4;  // locale decimal point
W void* DAT_6f8d0210; W void* DAT_6f8d0218; W void* DAT_6f8d021c; W void* DAT_6f8d0220; W void* DAT_6f8d0230;
W int32_t DAT_6f8cfdc4, DAT_6f8d0204, DAT_6f8d0208, DAT_6f8d0234, DAT_6f8d0250;
W int32_t DAT_6f8d01c8, DAT_6f8d01cc, DAT_6f8d01d4, DAT_6f8d01d8, DAT_6f8d01e0, DAT_6f8d01e4, DAT_6f8d01e8,
    DAT_6f8d01ec, DAT_6f8d024c;
W int32_t DAT_6f8ceb70[2], DAT_6f8ceb80[2];
W void* DAT_6f8d023c;  // CRITICAL_SECTION*
W uint8_t* DAT_6f8d0244;  // Bink handle
W char DAT_6f8cb718[4];

// Win32 / CRT / sibling-DLL callees
W uint32_t w32_LCMapStringA(uint32_t, uint32_t, const char*, int, char*, int);
W uint32_t w32_LCMapStringW(uint32_t, uint32_t, const uint16_t*, int, uint16_t*, int);
W uint32_t w32_MultiByteToWideChar(uint32_t, uint32_t, const char*, int, uint16_t*, int);
W uint32_t w32_WideCharToMultiByte(uint32_t, uint32_t, const uint16_t*, int, char*, int, const char*, int*);
W uint32_t w32_GetLastError(void);
W uint32_t w32_GetModuleFileNameA(void*, char*, uint32_t);
W void w32_EnterCriticalSection(void*); W void w32_LeaveCriticalSection(void*);
W void* FUN_6f8c5960(uint32_t cpFrom, uint32_t cpTo, const char* src, int* len, int* out, int outLen);
W uint32_t FUN_6f8c5919(uint32_t lcid);
W uint32_t ___isctype_mt(void* t, void* ptd, uint32_t c, int mask);
W void ___crtMessageBoxA(const char* text, const char* caption, uint32_t flags);
W void __exit(int);
W void q_d2ddraw_batch_b144_FUN_6f8c7690(void* hr, const char* msg);
W uint32_t Ordinal_10265(uint32_t); W void Ordinal_10024(const void*, uint32_t); W void Ordinal_10029(const char*, const char*);
W void Ordinal_10060(uint32_t h, int32_t* a, int32_t* b);  // reg: also sets ESI/EDI (see 0x6E20)
W void Ordinal_10000(void);
W int32_t Ordinal_10002(uint32_t), Ordinal_10069(uint32_t);
W void _BinkDoFrame_4(void*); W void _BinkNextFrame_4(void*);
W void _BinkCopyToBuffer_28(void*, void*, int, int, int, int, int);
W void FUN_6f8ca001(uint32_t lo, uint32_t hi, STRFLT* out, char* buf);  // _fltout2
W void __fptostr(char* buf, int digits, STRFLT* f);

// 1.13c D2DDraw+0x51d0: __crtLCMapStringA (wide path when LCMapStringW works, else codepage convert).
EMSCRIPTEN_KEEPALIVE uint32_t FUN_6f8c51d0(uint32_t lcid, uint32_t flags, const char* src, int cch, char* dst,
                                            int dstLen, uint32_t cp, int err) {
  if (DAT_6f8cf2a4 == 0) {
    static const uint16_t empty[1] = {0};
    if (w32_LCMapStringW(0, 0x100, empty, 1, 0, 0) == 0) {
      if (w32_GetLastError() == 0x78) DAT_6f8cf2a4 = 2;
    } else DAT_6f8cf2a4 = 1;
  }
  if (cch > 0) {  // strnlen
    int n = 0;
    while (n < cch && src[n]) n++;
    cch = (n < cch) ? n + 1 : n;
  }
  if (DAT_6f8cf2a4 == 2 || DAT_6f8cf2a4 == 0) {
    char* tmp = 0; char* conv = 0; uint32_t ret = 0;
    int32_t n = cch;
    if (lcid == 0) lcid = DAT_6f8cf264;
    if (cp == 0) cp = DAT_6f8cf274;
    uint32_t acp = FUN_6f8c5919(lcid);
    if (acp == 0xffffffff) return 0;
    if (acp == cp) return w32_LCMapStringA(lcid, flags, src, cch, dst, dstLen);
    char* in = (char*)FUN_6f8c5960(cp, acp, src, &n, 0, 0);
    if (!in) return 0;
    uint32_t len = w32_LCMapStringA(lcid, flags, in, n, 0, 0);
    if (len) {
      tmp = (char*)malloc(len);
      if (tmp) {
        memset(tmp, 0, len);
        int32_t l2 = (int32_t)len;
        len = w32_LCMapStringA(lcid, flags, in, n, tmp, l2);
        if (len) {
          l2 = (int32_t)len;
          conv = (char*)FUN_6f8c5960(acp, cp, tmp, &l2, (int*)dst, dstLen);
          ret = conv ? (uint32_t)l2 : 0;
        }
      }
    }
    free(in);
    free(tmp);
    if (conv && conv != dst) free(conv);
    return ret;
  }
  // DAT_6f8cf2a4 == 1
  if (cp == 0) cp = DAT_6f8cf274;
  int wl = (int)w32_MultiByteToWideChar(cp, (err != 0) * 8 + 1, src, cch, 0, 0);
  if (!wl) return 0;
  uint16_t* w = (uint16_t*)malloc(wl * 2);
  uint16_t* m = 0;
  uint32_t ret = 0;
  if (!w) return 0;
  if (w32_MultiByteToWideChar(cp, 1, src, cch, w, wl)) {
    ret = w32_LCMapStringW(lcid, flags, w, wl, 0, 0);
    if (ret) {
      if (flags & 0x400) {
        if (dstLen != 0 && (int)ret <= dstLen) w32_LCMapStringW(lcid, flags, w, wl, (uint16_t*)dst, dstLen);
      } else {
        m = (uint16_t*)malloc(ret * 2);
        if (m && w32_LCMapStringW(lcid, flags, w, wl, m, (int)ret))
          ret = w32_WideCharToMultiByte(cp, 0, m, (int)ret, (dstLen ? dst : 0), dstLen, 0, 0);
      }
    }
  }
  free(m);
  free(w);
  return ret;
}

// 1.13c D2DDraw+0x560c: __invoke_watson-style fatal ("Buffer overrun detected!"), then exit(3).
EMSCRIPTEN_KEEPALIVE void FUN_6f8c560c(int kind) {
  if (DAT_6f8cf2a8) { DAT_6f8cf2a8(); __exit(3); return; }
  const char* msg = kind == 1 ? "Buffer overrun detected!" : "Unknown security failure detected!";
  const char* tail = kind == 1 ? (const char*)0x6f8cc020 : (const char*)0x6f8cc0e0;  // retail rodata, text not recovered
  char name[0x104 + 4], out[0x200];
  char* p = name;
  if (!w32_GetModuleFileNameA(0, name, 0x104)) strcpy(name, "<program name unknown>");
  size_t n = strlen(p);
  if (n + 11 > 0x3c) { p = name + n - 0x31; strncpy(p, "...", 3); }
  strcpy(out, msg);
  strcat(out, DAT_6f8cb718);
  strcat(out, "Program: ");
  strcat(out, p);
  strcat(out, DAT_6f8cb718);
  strcat(out, tail);
  ___crtMessageBoxA(out, "Microsoft Visual C++ Runtime Library", 0x12010);
  __exit(3);
}

// 1.13c D2DDraw+0x5b74: ___tolower_mt. `t` = _ptiddata/locale struct (retail 32-bit layout, byte offsets).
static inline int32_t L(const uint8_t* t, int off) { int32_t v; memcpy(&v, t + off, 4); return v; }
EMSCRIPTEN_KEEPALIVE uint32_t b144_FUN_6f8c5b74(void* thisp, const uint8_t* t, uint32_t c) {
  if (L(t, 0x14) == 0 || (L(t, 0x24) != 0 && c < 0x80)) {
    if ((int32_t)c > 0x40 && (int32_t)c < 0x5b) return c + 0x20;
    return c;
  }
  const uint8_t* ctype = (const uint8_t*)(uintptr_t)(uint32_t)L(t, 0x48);  // ponytail: retail 32-bit ptr
  if (c < 0x100) {
    uint32_t u = L(t, 0x28) < 2 ? (ctype[c * 2] & 1) : ___isctype_mt(thisp, (void*)t, c, 1);
    if (!u) return c;
  }
  uint8_t in[4] = {0, 0, 0, 0}, out[4] = {0, 0, 0, 0};
  uint32_t n;
  if ((ctype[1 + ((c >> 8) & 0xff) * 2] & 0x80) == 0) { in[0] = (uint8_t)c; n = 1; }
  else { in[0] = (uint8_t)(c >> 8); in[1] = (uint8_t)c; n = 2; }
  n = FUN_6f8c51d0(L(t, 0x14), 0x100, (const char*)in, n, (char*)out, 3, L(t, 4), 1);
  if (!n) return c;
  return n == 1 ? out[0] : (uint32_t)(out[0] << 8 | out[1]);
}

// 1.13c D2DDraw+0x6e20: clip a rect (r[0..3]=l,r,t,b) against the screen; out[4]. reg: ecx=h, esi=lo, edi=x.
EMSCRIPTEN_KEEPALIVE uint32_t FUN_6f8c6e20(int32_t ecx, uint32_t obj, const int32_t* r, int32_t* out,
                                            int32_t esi, int32_t edi) {
  int32_t s8 = 0, s4 = 0;
  Ordinal_10060(obj, &s8, &s4);
  Ordinal_10000(); int32_t base = 0; (void)obj;
  uint32_t x0 = (uint32_t)((r[0] < 1) - 1) & (uint32_t)r[0];
  int32_t x1 = DAT_6f8d01c8 <= r[1] ? DAT_6f8d01c8 : r[1];
  if ((int32_t)x0 > x1) return 1;
  int32_t w = Ordinal_10002(obj);
  if (!(s8 + w < s4 && esi <= (int32_t)(x0 + s8 + w))) return 1;
  uint32_t y0 = (uint32_t)((r[2] < 1) - 1) & (uint32_t)r[2];
  int32_t y1 = DAT_6f8d01d8 <= r[3] ? DAT_6f8d01d8 : r[3];
  if ((int32_t)y0 > y1) return 1;
  int32_t v = ecx + Ordinal_10069(obj);
  if (base > v) return 1;
  if ((int32_t)(v - x0) > y1) return 1;
  out[0] = edi;
  out[1] = y0;
  out[2] = y1 < v ? (v - y1) - 1 : 0;
  out[3] = (int32_t)(v - x0) < base ? ((v - out[2]) - base) + 1 : (int32_t)x0 - out[2];
  if (out[3] < 1) { Ordinal_10024("", Ordinal_10265(0xab)); _exit(-1); }
  return 0;
}

// 1.13c D2DDraw+0x6ff0: per-frame Bink decode + copy into the primary surface. reg: edi = surface ptr (DDSURFACEDESC+0x24).
EMSCRIPTEN_KEEPALIVE void FUN_6f8c6ff0(void) {
  if (!DAT_6f8cfdc4) return;
  _BinkDoFrame_4(DAT_6f8d0244);
  if (!DAT_6f8d023c) { Ordinal_10024("", Ordinal_10265(0x42)); _exit(-1); }
  w32_EnterCriticalSection(DAT_6f8d023c);
  uint32_t d[27] = {0};
  d[0] = 0x6c;
  if (slot(DAT_6f8d0218, 100)(DAT_6f8d0218, 0, d, 0x21, 0) == 0) {
    _BinkCopyToBuffer_28(DAT_6f8d0244, (void*)(uintptr_t)d[9], (int)d[4], *(int32_t*)(DAT_6f8d0244 + 4),
                         DAT_6f8d01e4, DAT_6f8d01e0, DAT_6f8d024c);
    slot(DAT_6f8d0218, 0x80)(DAT_6f8d0218, 0);  // Unlock
  }
  w32_LeaveCriticalSection(DAT_6f8d023c);
  if (*(int32_t*)(DAT_6f8d0244 + 0xc) == *(int32_t*)(DAT_6f8d0244 + 8)) { DAT_6f8d0250 = 1; return; }
  _BinkNextFrame_4(DAT_6f8d0244);
}

// 1.13c D2DDraw+0x85a0: open the DirectDraw display; reg: mode = eax (0 = 640x480, else 800x600).
// Unverified: DDSURFACEDESC flags/caps the decompiler dropped (guessed per mode), SetCooperativeLevel flags arg.
EMSCRIPTEN_KEEPALIVE int32_t FUN_6f8c85a0(int mode) {
  if (DAT_6f8d0204 != 0 || !DAT_6f8d0210) { Ordinal_10024("", Ordinal_10265(0)); _exit(-1); }
  if (mode == 0) { DAT_6f8d01c8 = 0x280; DAT_6f8d01d8 = 0x1e0; } else { DAT_6f8d01c8 = 800; DAT_6f8d01d8 = 600; }
  DAT_6f8d01cc = DAT_6f8ceb80[mode];
  DAT_6f8d01d4 = DAT_6f8ceb70[mode];
  DAT_6f8d01e8 = 0;
  DAT_6f8d01ec = DAT_6f8d01c8;
  void* hr = slot(DAT_6f8d0210, 0x50)(DAT_6f8d0210, DAT_6f8cfdc4);
  const char* msg;
  if (hr && hr != (void*)0x88760245) { q_d2ddraw_batch_b144_FUN_6f8c7690(hr, "SetCooperativeLevel failed!"); return 0; }
  hr = slot(DAT_6f8d0210, 0x54)(DAT_6f8d0210, DAT_6f8d01d4, DAT_6f8d01cc);
  uint32_t d[27];
  uint32_t caps = 4;  // DDSCAPS_BACKBUFFER (guessed)
  if (hr) { msg = "SetDisplayMode failed!"; goto fail; }
  // try flip chain (2), then single backbuffer (1), then single buffer (0)
  for (int m = 2; m >= 0; m--) {
    memset(d, 0, sizeof d); d[0] = 0x6c;
    if (m == 2) { d[1] = 0x21; d[5] = 1; d[26] = 0x218; }       // CAPS|BACKBUFFERCOUNT; PRIMARY|FLIP|COMPLEX (guessed)
    else if (m == 1) { d[1] = 0x21; d[5] = 1; d[26] = 0x208; }  // PRIMARY|COMPLEX (guessed)
    else { d[1] = 1; d[26] = 0x200; }                           // PRIMARY
    DAT_6f8d0234 = m;
    hr = slot(DAT_6f8d0210, 0x18)(DAT_6f8d0210, d, &DAT_6f8d0218, 0);
    if (!hr) break;
    if (m == 0) { msg = "CreateSurface (primary single buffered) failed!"; goto fail; }
  }
  if (DAT_6f8d0234 == 0 || (hr = slot(DAT_6f8d0218, 0x30)(DAT_6f8d0218, &caps, &DAT_6f8d021c)) != 0) {
    if (DAT_6f8d0234 != 0) { msg = "GetAttachedSurface (backbuffer) failed!"; goto fail; }
    memset(d, 0, sizeof d);
    hr = slot(DAT_6f8d0218, 0x58)(DAT_6f8d0218, d);
    if (hr) { msg = "GetSurfaceDesc (primary) failed!"; goto fail; }
    hr = slot(DAT_6f8d0210, 0x18)(DAT_6f8d0210, d, &DAT_6f8d0220, 0);
    if (hr) { msg = "CreateSurface (render) failed!"; goto fail; }
    hr = slot(DAT_6f8d0218, 0x7c)(DAT_6f8d0218, DAT_6f8d0230);
    if (hr) { msg = "SetPalette failed!"; goto fail; }
    DAT_6f8d0204 = 1; DAT_6f8d0208 = 0;
    return 1;
  }
  // backbuffer found: fall into the same render-surface setup
  memset(d, 0, sizeof d);
  hr = slot(DAT_6f8d0218, 0x58)(DAT_6f8d0218, d);
  if (hr) { msg = "GetSurfaceDesc (primary) failed!"; goto fail; }
  hr = slot(DAT_6f8d0210, 0x18)(DAT_6f8d0210, d, &DAT_6f8d0220, 0);
  if (hr) { msg = "CreateSurface (render) failed!"; goto fail; }
  hr = slot(DAT_6f8d0218, 0x7c)(DAT_6f8d0218, DAT_6f8d0230);
  if (hr) { msg = "SetPalette failed!"; goto fail; }
  DAT_6f8d0204 = 1; DAT_6f8d0208 = 0;
  return 1;
fail:
  q_d2ddraw_batch_b144_FUN_6f8c7690(hr, msg);
  Ordinal_10029("Opening DirectDraw window failed!", "");
  return 0;
}

// 1.13c D2DDraw+0x9779: __shift -- move the string at p up by n bytes (n==0 no-op).
static void b144_shift(char* p, int n) { if (n) memmove(p + n, p, strlen(p) + 1); }
EMSCRIPTEN_KEEPALIVE void __shift_b144(char* p, int n) { b144_shift(p, n); }

// 1.13c D2DDraw+0x9796: __cftoe2. reg: f=eax (STRFLT), buf=ebx. The __shift args are reg-inferred (unverified).
static void cftoe2(STRFLT* f, char* buf, int nd, int caps, char g) {
  if (g) b144_shift(buf + (f->sign == 0x2d), 1);
  if (f->sign == 0x2d) *buf++ = '-';
  char* p = buf;
  if (nd > 0) { p = buf + 1; buf[0] = buf[1]; buf[1] = (char)DAT_6f8ce7e4; }
  p = strcpy(p + nd + (g == 0), "e+000");
  if (caps) p[0] = 'E';
  if (*f->mant != '0') {
    int e = f->decpt - 1;
    if (e < 0) { e = -e; p[1] = '-'; }
    if (e > 99) { p[2] += e / 100; e %= 100; }
    if (e > 9) { p[3] += e / 10; e %= 10; }
    p[4] += e;
  }
}
EMSCRIPTEN_KEEPALIVE void __cftoe2_b144(STRFLT* f, char* buf, int nd, int caps, char g) { cftoe2(f, buf, nd, caps, g); }

// 1.13c D2DDraw+0x98b4: __cftof2. reg: f=eax.
static char* cftof2(STRFLT* f, char* buf, int nd, char g) {
  int d = f->decpt;
  if (g && (uint32_t)(d - 1) == (uint32_t)nd) {
    buf[(f->sign == 0x2d) + (d - 1)] = '0';
    buf[(f->sign == 0x2d) + (d - 1) + 1] = 0;
  }
  char* p = buf;
  if (f->sign == 0x2d) *p++ = '-';
  if (f->decpt < 1) { b144_shift(p, 1); *p++ = '0'; } else p += f->decpt;
  if (nd > 0) {
    b144_shift(p, 1);
    *p = (char)DAT_6f8ce7e4;
    if (f->decpt < 0) {
      int z = -f->decpt;
      if (g || z <= nd) nd = z;
      b144_shift(p + 1, nd);
      memset(p + 1, '0', nd);
    }
  }
  return buf;
}
EMSCRIPTEN_KEEPALIVE char* __cftof2_b144(STRFLT* f, char* buf, int nd, char g) { return cftof2(f, buf, nd, g); }

// 1.13c D2DDraw+0x9844: _cftoe -- `%e` of the double at d into buf.
EMSCRIPTEN_KEEPALIVE char* b144_FUN_6f8c9844(const uint32_t* d, char* buf, int nd, int caps) {
  STRFLT f; char tmp[24];
  FUN_6f8ca001(d[0], d[1], &f, tmp);
  __fptostr(buf + (nd > 0) + (f.sign == 0x2d), nd + 1, &f);
  cftoe2(&f, buf, nd, caps, 0);
  return buf;
}

// 1.13c D2DDraw+0x9950: _cftof -- `%f` of the double at d into buf.
EMSCRIPTEN_KEEPALIVE char* b144_FUN_6f8c9950(const uint32_t* d, char* buf, int nd) {
  STRFLT f; char tmp[24];
  FUN_6f8ca001(d[0], d[1], &f, tmp);
  __fptostr(buf + (f.sign == 0x2d), f.decpt + nd, &f);
  cftof2(&f, buf, nd, 0);
  return buf;
}

// 1.13c D2DDraw+0x99b7: _cftog -- `%g` of the double at d into buf.
EMSCRIPTEN_KEEPALIVE void b144_FUN_6f8c99b7(const uint32_t* d, char* buf, int nd, int caps) {
  STRFLT f; char tmp[24];
  FUN_6f8ca001(d[0], d[1], &f, tmp);
  int e0 = f.decpt - 1;
  __fptostr(buf + (f.sign == 0x2d), nd, &f);
  int e = f.decpt - 1;
  if (e < -4 || nd <= e) { cftoe2(&f, buf, nd, caps, 1); return; }
  char* p = buf + (f.sign == 0x2d);
  if (e0 < e) { while (*p++) {} p[-2] = 0; }
  cftof2(&f, buf, nd, 1);
}
}  // extern "C"
