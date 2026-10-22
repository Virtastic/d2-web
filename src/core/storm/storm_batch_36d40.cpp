// src/core/storm/storm_batch_36d40.cpp -- 1.13c Storm.dll rows reimplemented from the cached
// decompiles (Stage A: implementation + marker, no spec, no vector). Teardown helpers
// (0x36D40, 0x38130, 0x38F90), the SCode return fixup (0x37E80) and the SBMP/SBLT image helpers
// (0x31E80, 0x320B0, 0x328C0, 0x32170, 0x32710, 0x33070). Retail 32-bit field offsets are kept;
// hidden register parameters (EAX/ESI/EDI) are explicit, in decompiler order. Unmapped callees
// and Storm globals are seams. Skipped: 0x32C00 and 0x32EC0 (decompile loses the BMP info-header
// locals / the RLE callee's register arguments, so no faithful body).
#include <cstdint>
#include <cstring>
#include <Windows.h>

namespace storm_b36d40 {

// ---- seams (retail callee / global, argument order) ----
void  Storm_Free(void* p, const char* tag, int32_t line);          // 0x0B960
void  Storm_Sub4D60(void* p);                                      // 0x04D60
void  Storm_Sub1FB30(const char* name, const char* arg);           // 0x1FB30
void  Storm_Sub1090(void* p);                                      // 0x01090
void* Storm_Alloc(uint32_t n, const char* file, int32_t line, uint32_t flags);  // 0x0C580
void  Storm_Sub21C40(uint8_t* a, int32_t b, const uint8_t* c, uint8_t* d, uint32_t w, uint32_t e, uint32_t bpp);
void  Storm_Sub21FF0(uint32_t ecx, void* dst, const void* src, int32_t h, uint32_t stride, uint32_t w);
int32_t Storm_Sub22690(const uint8_t* esi);
int32_t Storm_Sub225C0(const uint8_t* esi, const uint8_t* p, int32_t n, void* pal);
int32_t Storm_Sub222F0(const uint8_t* p, const uint8_t* esi, int32_t n, uint8_t* out);
uint32_t Storm_Sub22D50(const void* src, void* dst);
extern int32_t* g_trans_list;       // 0x6FC458B8
extern uint32_t g_trans_a;          // 0x6FC45364
extern int32_t  g_trans_b;          // 0x6FC45368
extern uint32_t g_trans_c;          // 0x6FC45360
extern int32_t  g_stream_list;      // 0x6FC45878
extern CRITICAL_SECTION g_stream_cs;// 0x6FC45858
extern int32_t  g_lastError;        // 0x6FC45388

static uint32_t rd32(const void* p, uintptr_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static uint16_t rd16(const void* p, uintptr_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static void wr16(uint8_t* p, uintptr_t o, uint16_t v) { std::memcpy(p + o, &v, 2); }
static void setErr(int32_t e) { g_lastError = e; SetLastError(e); }

static const char* kTrans = "..\\3rdParty\\STORM\\SOURCE\\STRANS.CPP";
static const char* kBlt = "..\\3rdParty\\STORM\\SOURCE\\SBLT.CPP";
static const char* kBmp = "..\\3rdParty\\STORM\\SOURCE\\SBMP.CPP";

// 1.13c Storm+0x36D40
int32_t Storm_Sub36D40(void) {
  while (g_trans_list && (intptr_t)g_trans_list > 0) {
    int32_t* n = g_trans_list;
    Storm_Sub1FB30("HSTRANS", nullptr);
    if (n[2] != 0) {
      if (g_trans_a) Storm_Free((void*)(uintptr_t)g_trans_a, kTrans, 0x2ec);
      g_trans_a = (uint32_t)n[2];
      g_trans_b = n[3];
      n[2] = 0;
      n[3] = 0;
    }
    Storm_Sub4D60(n);
    Storm_Free(n, "AUTRANS", -2);
  }
  if (g_trans_c) { Storm_Free((void*)(uintptr_t)g_trans_c, kTrans, 0x2fe); g_trans_c = 0; }
  if (g_trans_a) { Storm_Free((void*)(uintptr_t)g_trans_a, kTrans, 0x302); g_trans_a = 0; g_trans_b = 0; }
  return 1;
}

static void freeStream(void* s) {
  EnterCriticalSection(&g_stream_cs);
  Storm_Sub1090(s);
  Storm_Free(s, "AUSTREAM", -2);
  LeaveCriticalSection(&g_stream_cs);
}

// 1.13c Storm+0x38130
void Storm_Sub38130(uint8_t* obj) {
  uint32_t* slot = (uint32_t*)(obj + 0x18);
  for (int i = 0; i < 2; i++, slot += 3) {
    if (slot[0]) freeStream((void*)(uintptr_t)slot[0]);
    slot[0] = slot[1] = slot[2] = 0;
  }
  uint32_t tbl = rd32(obj, 0x30);
  if (tbl) {
    uint8_t* t = (uint8_t*)(uintptr_t)tbl;
    for (int o = 0; o < 0xc00; o += 0xc) {
      uint32_t s = rd32(t, o + 8);
      if (s) freeStream((void*)(uintptr_t)s);
    }
    Storm_Free(t, kBlt, 0x72);
    std::memset(obj + 0x30, 0, 4);
  }
}

// 1.13c Storm+0x38F90
int32_t Storm_Sub38F90(void) {
  EnterCriticalSection(&g_stream_cs);
  while (g_stream_list > 0) {
    Storm_Sub1FB30("HSCODESTREAM", nullptr);
    void* s = (void*)(uintptr_t)g_stream_list;  // (n<1 ? 0 : n) & n == n here
    freeStream(s);
  }
  LeaveCriticalSection(&g_stream_cs);
  return 1;
}

// 1.13c Storm+0x37E80  (EAX = code, param_2 = out)
void Storm_Sub37E80(int32_t* code, uint32_t* out) {
  int32_t c = *code;
  if (c == 0x42) { *code = 0xf00021; *out = 0; }
  else if (c == 0xf0001) { *code = 0xf00021; *out = (*out == 0); }
  else if (c == 0xff0062) { *code = 0xf00021; *out = 0xffffffff; }
}

// 1.13c Storm+0x31E80  (EAX = pw, ECX = hdr, EDX = ph)
int32_t Storm_Sub31E80(uint32_t* pw, const uint8_t* hdr, int32_t* ph, int32_t p3, uint8_t* pal,
                       uint8_t* pix, uint32_t p6, int32_t* pbpp) {
  (void)p6;
  uint16_t top = rd16(hdr, 6), bot = rd16(hdr, 10);
  uint32_t w = (uint32_t)rd16(hdr, 8) - rd16(hdr, 4) + 1;
  int32_t bpp = (int32_t)hdr[3] * hdr[0x41];
  if (pw) *pw = w;
  if (ph) *ph = (int32_t)bot - top + 1;
  if (pbpp) *pbpp = bpp;
  if (pix)
    Storm_Sub21C40((uint8_t*)hdr + 0x80, (p3 - (bpp == 8 ? 0x301 : 0)) - 0x80, pix,
                   (uint8_t*)hdr + 0x80, w, rd16(hdr, 0x42), hdr[0x41]);
  if (pal && bpp == 8) {
    const uint8_t* s = hdr - 0x300 + p3;
    for (int i = 0; i < 0x100; i++, s += 3, pal += 4) { pal[0] = s[0]; pal[1] = s[1]; pal[2] = s[2]; pal[3] = 0; }
  }
  return 1;
}

// 1.13c Storm+0x320B0  (EAX = pw, EDI = bmp header; p3 = palette out)
int32_t Storm_Sub320B0(uint32_t* pw, int32_t* ph, uint32_t* pbpp, uint8_t* pal, uint8_t* pix,
                       int32_t p5, const uint8_t* bmp) {
  if (pw) *pw = rd32(bmp, 0x12);
  if (ph) *ph = (int32_t)rd32(bmp, 0x16);
  uint16_t bpp = rd16(bmp, 0x1c);
  if (pbpp) *pbpp = bpp;
  if (bpp < 9) {
    if (pal) {
      const uint8_t* s = bmp + 0x36;
      for (int i = 0; i < 0x100; i++, s += 4, pal += 4) { pal[0] = s[2]; pal[1] = s[1]; pal[2] = s[0]; pal[3] = 0; }
    }
  } else pal = nullptr;
  bool hasPal = pal != nullptr;
  if (pix) {
    uint32_t row = (uint32_t)((int32_t)(bpp * rd32(bmp, 0x12)) >> 3), stride = row;
    if (row & 3) stride += 4 - (row & 3);
    Storm_Sub21FF0(stride, pix, bmp + (rd32(bmp, 10) - (hasPal ? 0x400 : 0)) - 0x36, p5, row, stride);
  }
  return 1;
}

// 1.13c Storm+0x328C0  (ESI = data)
int32_t Storm_Sub328C0(uint32_t size, void* pal, uint8_t* pix, const uint8_t* esi, uint32_t* pw,
                       uint32_t* ph, uint32_t* pbpp) {
  if (size < 0x12 || Storm_Sub22690(esi) == 0) return 0;
  uint16_t h = rd16(esi, 0xe);
  uint8_t bits = esi[0x10];
  int32_t palBytes = esi[1] ? ((esi[7] + 7) >> 3) * (int32_t)rd16(esi, 5) : 0;
  int32_t pixBytes = esi[2] < 9 ? ((bits + 7) >> 3) * (int32_t)h * rd16(esi, 0xc) : 1;
  if ((uint32_t)esi[0] + pixBytes + 0x12 + palBytes > size) return 0;
  if (pw) *pw = rd16(esi, 0xc);
  if (ph) *ph = h;
  if (pbpp) *pbpp = bits;
  if (pal && esi[1] && Storm_Sub225C0(esi, esi + esi[0] + 0x12, palBytes, pal) == 0) return 0;
  if (pix) {
    const uint8_t* data = esi + esi[0] + palBytes + 0x12;
    if (Storm_Sub222F0(data, esi, (int32_t)((esi + size) - data), pix) == 0) return 0;
  }
  return 1;
}

// 1.13c Storm+0x32170  (EAX = bpp; p2 = RGBA palette; p3 = pixels)
int32_t Storm_Sub32170(int32_t bpp, HANDLE h, const uint8_t* pal, const void* pix, int32_t width,
                       int32_t height, uint32_t stride) {
  uint8_t hd[0x12] = {0};
  DWORD n;
  hd[1] = bpp == 8;
  hd[2] = bpp == 8 ? 1 : 2;
  if (bpp == 8) { wr16(hd, 5, 0x100); hd[7] = 0x18; }
  wr16(hd, 0xc, (uint16_t)width);
  wr16(hd, 0xe, (uint16_t)height);
  hd[0x10] = (uint8_t)bpp;
  hd[0x11] = (bpp == 0x20 ? 8 : 0) | 0x20;
  WriteFile(h, hd, 0x12, &n, nullptr);
  if (bpp == 8) {
    uint8_t p[0x300];
    for (int i = 0; i < 0x100; i++) { p[i*3] = pal[i*4+2]; p[i*3+1] = pal[i*4+1]; p[i*3+2] = pal[i*4]; }
    WriteFile(h, p, 0x300, &n, nullptr);
  }
  DWORD row = ((bpp + 7) / 8) * width;
  if (stride != row) {
    for (; height; height--) { WriteFile(h, pix, row, &n, nullptr); pix = (const uint8_t*)pix + stride; }
    return 1;
  }
  WriteFile(h, pix, row * height, &n, nullptr);
  return 1;
}

// 1.13c Storm+0x32710  (EAX = path; returns extension dot, else file-name start)
char* Storm_Sub32710(char* p) {
  if (!p) { setErr(0x57); return nullptr; }  // retail then dereferences 1; not reproduced
  for (;; p++) {
    char* q = p;
    while (*q && *q != '\\') q++;
    if (!*q) break;
    p = q;
  }
  for (;;) {
    char* s = p + 1;
    if (!*s) return p;
    char* d = std::strchr(s, '.');
    if (!d) return p;
    p = d;
  }
}

// 1.13c Storm+0x33070  (EAX = height, ESI = file; p1 = RGBA palette, p2 = pixels, p3 = width)
int32_t Storm_Sub33070(int32_t height, HANDLE h, const uint8_t* pal, const void* pix, int32_t width) {
  DWORD n;
  uint8_t hd[13] = {'G','I','F','8','7','a'};
  wr16(hd, 6, (uint16_t)width);
  wr16(hd, 8, (uint16_t)height);
  hd[10] = 0xf7;
  WriteFile(h, hd, 13, &n, nullptr);
  uint8_t p[0x300];
  for (int i = 0; i < 0x100; i++) { p[i*3] = pal[i*4]; p[i*3+1] = pal[i*4+1]; p[i*3+2] = pal[i*4+2]; }
  WriteFile(h, p, 0x300, &n, nullptr);
  uint8_t b = 0x2c;
  WriteFile(h, &b, 1, &n, nullptr);
  uint8_t d[10] = {0};
  wr16(d, 4, (uint16_t)width);
  wr16(d, 6, (uint16_t)height);
  d[9] = 8;  // flags 0, LZW min code size 8 (word 0x0800)
  WriteFile(h, d, 10, &n, nullptr);
  void* buf = Storm_Alloc((uint32_t)(width * height * 2), kBmp, 1000, 0);
  DWORD len = Storm_Sub22D50(pix, buf);
  WriteFile(h, buf, len, &n, nullptr);
  Storm_Free(buf, kBmp, 0x3ee);
  b = 0; WriteFile(h, &b, 1, &n, nullptr);
  b = 0x3b; WriteFile(h, &b, 1, &n, nullptr);
  return 1;
}

}  // namespace storm_b36d40
