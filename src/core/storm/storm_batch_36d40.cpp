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

// ---- real ports this file binds to (global scope; each keeps its own file) ----
extern "C" bool Storm_Sub31FF0(int32_t rows, uint8_t* dst, const uint8_t* src, int32_t dstSize, int32_t rowBytes, int32_t stride, int32_t* lastError);  // storm_sub_31ff0.cpp
extern "C" void Storm_Sub31C40(uint32_t dstLen, uint32_t srcLen, uint8_t* dst, uint8_t* src, uint32_t width, uint32_t stride, int32_t ch);  // storm_sub_31c40.cpp
extern "C" uint32_t Storm_Sub32690(const uint8_t* header);                                  // storm_image_leaves.cpp (TGA/PCX header check)
uint32_t Storm_Sub325C0(const uint8_t* hdr, const uint8_t* src, int srcLen, uint8_t* out);       // storm_sub_325c0.cpp (palette unpack)
extern "C" uint32_t Storm_Sub322F0(const uint8_t* src, const uint8_t* hdr, int32_t srcLen, uint8_t* dst, uint32_t limit);  // storm_sub_322f0.cpp
extern "C" int32_t Storm_Sub32D50(const uint8_t* src, uint8_t* dst, int32_t n);                // storm_sub_32d50.cpp (LZW encode)
extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);   // storm_alloc_1b960.cpp (SMemFree)
extern "C" void*   Storm_Sub1C580(uint32_t size, const char* tag, int32_t line, uint32_t flags);  // same file (SMemAlloc)
void Storm_Sub4D60(uint8_t* node);                                 // storm_batch_b47.cpp (list unlink)
extern "C" void storm_port_1090(uint32_t* r);                      // gfx/storm_batch_1000.cpp: r[0] = node, Storm+0x1090 (SCODE release)
void Storm_Sub2FB30(const char* name, const char* detail);         // storm_2fb30.cpp (handle-leak report)
CRITICAL_SECTION* Storm_StreamLockCS();                            // storm_batch_34110.cpp: AUSTREAM lock, 0x6FC45858
void Storm_SetError45388(uint32_t e);                              // storm_batch_26e20.cpp: last error 0x6FC45388 + SetLastError
uint32_t*         Storm_StreamTailWord();                          // storm_batch_34110.cpp: AUSTREAM tail, 0x6FC45878

namespace storm_b36d40 {

// ---- seams (retail callee / global, argument order), bound to the ports above ----
inline void Storm_Free(void* p, const char* tag, int32_t line) { (void)Storm_Sub1B960(p, tag, line); }  // 0x0B960
inline void Storm_Sub4D60(void* p) { ::Storm_Sub4D60((uint8_t*)p); }                                  // 0x04D60
inline void Storm_Sub1FB30(const char* name, const char* arg) { ::Storm_Sub2FB30(name, arg); }        // 0x2FB30
inline void Storm_Sub1090(void* p) { uint32_t r[5] = {(uint32_t)(uintptr_t)p, 0, 0, 0, 0}; storm_port_1090(r); }  // 0x01090
inline void* Storm_Alloc(uint32_t n, const char* file, int32_t line, uint32_t flags) { return Storm_Sub1C580(n, file, line, flags); }  // 0x0C580
// Transport globals (STRANS): no port writes them yet, so they read as zero (the list is empty).
int32_t* g_trans_list = nullptr;    // 0x6FC458B8 head of the HSTRANS list
uint32_t g_trans_a = 0;             // 0x6FC45364
int32_t  g_trans_b = 0;             // 0x6FC45368
uint32_t g_trans_c = 0;             // 0x6FC45360
static int32_t streamList() { return (int32_t)*Storm_StreamTailWord(); }   // 0x6FC45878

static uint32_t rd32(const void* p, uintptr_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static uint16_t rd16(const void* p, uintptr_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static void wr16(uint8_t* p, uintptr_t o, uint16_t v) { std::memcpy(p + o, &v, 2); }
static void setErr(int32_t e) { Storm_SetError45388((uint32_t)e); }   // 0x6FC45388 + SetLastError

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
  EnterCriticalSection(Storm_StreamLockCS());
  Storm_Sub1090(s);
  Storm_Free(s, "AUSTREAM", -2);
  LeaveCriticalSection(Storm_StreamLockCS());
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
  EnterCriticalSection(Storm_StreamLockCS());
  while (streamList() > 0) {
    Storm_Sub1FB30("HSCODESTREAM", nullptr);
    void* s = (void*)(uintptr_t)streamList();  // (n<1 ? 0 : n) & n == n here
    freeStream(s);
  }
  LeaveCriticalSection(Storm_StreamLockCS());
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
  uint16_t top = rd16(hdr, 6), bot = rd16(hdr, 10);
  uint32_t w = (uint32_t)rd16(hdr, 8) - rd16(hdr, 4) + 1;
  int32_t h = (int32_t)bot - top + 1;
  int32_t bpp = (int32_t)hdr[3] * hdr[0x41];
  if (pw) *pw = w;
  if (ph) *ph = h;
  if (pbpp) *pbpp = bpp;
  if (pix) {
    uint32_t dstLen = (uint32_t)(((bpp + 7) / 8) * h * (int32_t)w);  // retail: min(limit, bytes per pixel * h * w)
    if (p6 < dstLen) dstLen = p6;
    ::Storm_Sub31C40(dstLen, (uint32_t)((p3 - (bpp == 8 ? 0x301 : 0)) - 0x80), pix, (uint8_t*)hdr + 0x80, w,
                     rd16(hdr, 0x42), hdr[0x41]);
  }
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
    int32_t err = 0;  // retail ignores the result (and SetLastError 0x57 on bad args)
    ::Storm_Sub31FF0((int32_t)rd32(bmp, 0x16), pix, bmp + (rd32(bmp, 10) - (hasPal ? 0x400 : 0)) - 0x36, p5, (int32_t)row, (int32_t)stride, &err);
  }
  return 1;
}

// 1.13c Storm+0x328C0  (ESI = data; `limit` is the 4th stack word, handed to the unpacker in EAX)
int32_t Storm_Sub328C0(uint32_t size, void* pal, uint8_t* pix, uint32_t limit, const uint8_t* esi, uint32_t* pw,
                       uint32_t* ph, uint32_t* pbpp) {
  if (size < 0x12 || ::Storm_Sub32690(esi) == 0) return 0;
  uint16_t h = rd16(esi, 0xe);
  uint8_t bits = esi[0x10];
  int32_t palBytes = esi[1] ? ((esi[7] + 7) >> 3) * (int32_t)rd16(esi, 5) : 0;
  int32_t pixBytes = esi[2] < 9 ? ((bits + 7) >> 3) * (int32_t)h * rd16(esi, 0xc) : 1;
  if ((uint32_t)esi[0] + pixBytes + 0x12 + palBytes > size) return 0;
  if (pw) *pw = rd16(esi, 0xc);
  if (ph) *ph = h;
  if (pbpp) *pbpp = bits;
  if (pal && esi[1] && ::Storm_Sub325C0(esi, esi + esi[0] + 0x12, palBytes, (uint8_t*)pal) == 0) return 0;
  if (pix) {
    const uint8_t* data = esi + esi[0] + palBytes + 0x12;
    if (::Storm_Sub322F0(data, esi, (int32_t)((esi + size) - data), pix, limit) == 0) return 0;
  }
  return 1;
}

// 1.13c Storm+0x32170  (EAX = bpp; p2 = RGBA palette; p3 = pixels)
template <class W>
static int32_t Tga32170(int32_t bpp, W put, const uint8_t* pal, const void* pix, int32_t width, int32_t height, uint32_t stride) {
  uint8_t hd[0x12] = {0};
  hd[1] = bpp == 8;
  hd[2] = bpp == 8 ? 1 : 2;
  if (bpp == 8) { wr16(hd, 5, 0x100); hd[7] = 0x18; }
  wr16(hd, 0xc, (uint16_t)width);
  wr16(hd, 0xe, (uint16_t)height);
  hd[0x10] = (uint8_t)bpp;
  hd[0x11] = (bpp == 0x20 ? 8 : 0) | 0x20;
  put(hd, 0x12);
  if (bpp == 8) {
    uint8_t p[0x300];
    for (int i = 0; i < 0x100; i++) { p[i*3] = pal[i*4+2]; p[i*3+1] = pal[i*4+1]; p[i*3+2] = pal[i*4]; }
    put(p, 0x300);
  }
  uint32_t row = (uint32_t)(((bpp + 7) / 8) * width);
  if (stride != row) {
    for (; height; height--) { put(pix, row); pix = (const uint8_t*)pix + stride; }
    return 1;
  }
  put(pix, row * (uint32_t)height);
  return 1;
}
int32_t Storm_Sub32170(int32_t bpp, HANDLE h, const uint8_t* pal, const void* pix, int32_t width,
                       int32_t height, uint32_t stride) {
  return Tga32170(bpp, [h](const void* q, uint32_t n) { DWORD w; WriteFile(h, q, n, &w, nullptr); }, pal, pix, width, height, stride);
}
int32_t Storm_Sub32170Sink(int32_t bpp, uint8_t* sink, size_t cap, size_t* len, const uint8_t* pal, const void* pix,
                           int32_t width, int32_t height, uint32_t stride) {
  size_t at = 0;
  int32_t r = Tga32170(bpp, [&](const void* q, uint32_t n) { if (at + n <= cap) memcpy(sink + at, q, n); at += n; }, pal, pix, width, height, stride);
  *len = at;
  return r;
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
template <class W>
static int32_t Gif33070(int32_t height, W put, const uint8_t* pal, const void* pix, int32_t width) {
  uint8_t hd[13] = {'G','I','F','8','7','a'};
  wr16(hd, 6, (uint16_t)width);
  wr16(hd, 8, (uint16_t)height);
  hd[10] = 0xf7;
  put(hd, 13);
  uint8_t p[0x300];
  for (int i = 0; i < 0x100; i++) { p[i*3] = pal[i*4]; p[i*3+1] = pal[i*4+1]; p[i*3+2] = pal[i*4+2]; }
  put(p, 0x300);
  uint8_t b = 0x2c;
  put(&b, 1);
  uint8_t d[10] = {0};
  wr16(d, 4, (uint16_t)width);
  wr16(d, 6, (uint16_t)height);
  d[9] = 8;  // flags 0, LZW min code size 8 (word 0x0800)
  put(d, 10);
  void* buf = Storm_Alloc((uint32_t)(width * height * 2), kBmp, 1000, 0);
  uint32_t len = (uint32_t)::Storm_Sub32D50((const uint8_t*)pix, (uint8_t*)buf, width * height);
  put(buf, len);
  Storm_Free(buf, kBmp, 0x3ee);
  b = 0; put(&b, 1);
  b = 0x3b; put(&b, 1);
  return 1;
}
int32_t Storm_Sub33070(int32_t height, HANDLE h, const uint8_t* pal, const void* pix, int32_t width) {
  return Gif33070(height, [h](const void* q, uint32_t n) { DWORD w; WriteFile(h, q, n, &w, nullptr); }, pal, pix, width);
}
int32_t Storm_Sub33070Sink(int32_t height, uint8_t* sink, size_t cap, size_t* len, const uint8_t* pal, const void* pix, int32_t width) {
  size_t at = 0;
  int32_t r = Gif33070(height, [&](const void* q, uint32_t n) { if (at + n <= cap) memcpy(sink + at, q, n); at += n; }, pal, pix, width);
  *len = at;
  return r;
}

}  // namespace storm_b36d40

extern "C" uint32_t Storm_Sub32170Probe(int32_t bpp, const uint8_t* pal, const void* pix, int32_t width, int32_t height,
                                        uint32_t stride, uint8_t* sink, size_t cap) {
  size_t n; return (uint32_t)storm_b36d40::Storm_Sub32170Sink(bpp, sink, cap, &n, pal, pix, width, height, stride);
}
extern "C" uint32_t Storm_Sub33070Probe(int32_t height, const uint8_t* pal, const void* pix, int32_t width, uint8_t* sink, size_t cap) {
  size_t n; return (uint32_t)storm_b36d40::Storm_Sub33070Sink(height, sink, cap, &n, pal, pix, width);
}
