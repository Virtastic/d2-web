// Fog implementation batch b69 (decompile-derived). Register-arg hidden inputs are leading params.
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <initializer_list>

extern "C" {
void FUN_6ff5fc20(void);
void Ordinal_116(void);
void FUN_6ff60f30(int, const char*, int, const char*);
void FUN_6ff684a0(void);
int Ordinal_502(const char*, int, int);
void* FUN_6ff6b560(int*);
void FUN_6ff6b0c0(void**);
void* FUN_6ff6cda0(int, void*, int, const char*, int);
int Ordinal_508(const char*, const char*, int);
void* Ordinal_491(void*, const void*, size_t);
void Ordinal_501(void*, const void*, int);
void FUN_6ff62020(void*);
void FUN_6ff6efd0(uint8_t*);
extern const uint8_t DAT_6ff76690[];
extern uint8_t DAT_6ff7f210[];
extern const uint32_t DAT_6ff7f310[];
extern uint8_t* DAT_6ff806d0[];
extern uint32_t DAT_6ff80ed0[0x1fe + 2];
extern uint32_t DAT_6ff9af48[6];
extern const char DAT_6ff768ec[];
}

[[noreturn]] static void fog_fatal(int line) {
  FUN_6ff5fc20();
  Ordinal_116();
  FUN_6ff60f30(6, "", line, "Unrecoverable internal error %08x");
  FUN_6ff684a0();
  _Exit(-1);
}

struct ExcelRaw { char* base; char* data; int rows; int cols; };
struct ExcelIdx { int count; int cap; uint32_t (*ents)[2]; int flag; };
struct ExcelLink { int next; };

// 1.13c Fog+0x172B0  (EBX = list, ESI = third value hidden)
int Fog_172B0(uint8_t* list, const char* name, uint32_t v1, uint32_t v2) {
  uint32_t h = Ordinal_502(name, 0, 0);
  int n = *(int*)(list + 4);
  int off = n * 0xc;
  if (0x400 < (unsigned)(off + 0x14)) return 0xe;
  *(uint32_t*)(list + off + 8) = h;
  *(uint32_t*)(list + off + 0xc) = v1;
  *(uint32_t*)(list + off + 0x10) = v2;
  *(int*)(list + 4) = n + 1;
  return 0;
}

// 1.13c Fog+0x17170  (ESI = out2, EDI = out1 hidden)
int Fog_17170(const char* name, const uint8_t* list, uint32_t* out1, uint32_t* out2) {
  uint32_t h = Ordinal_502(name, 0, 0);
  const uint32_t* p = (const uint32_t*)(list + 8);
  for (uint32_t i = 0; i < *(const uint32_t*)(list + 4); i++, p += 3)
    if (p[0] == h) { *out1 = p[1]; *out2 = p[2]; return 0; }
  return 0x16;
}

// 1.13c Fog+0x12770
ExcelRaw* Fog_12770(char* buf, int size) {
  char* end = buf + size;
  ExcelRaw* r = (ExcelRaw*)FUN_6ff6b560(nullptr);
  if (!r) FUN_6ff60f30(2, "..\\Source\\Fog\\Src\\Excel\\Excel.cpp", 0x4d, DAT_6ff768ec);
  r->base = buf; r->data = nullptr; r->rows = 0; r->cols = 0;
  char* p = buf;
  for (;;) {
    if (end <= p) goto fail;
    char c = *p++;
    if (c == '\t') { p[-1] = 0; r->cols++; }
    else if (c == '\r') { p[-1] = 0; break; }
  }
  if (*p != '\n') fog_fatal(0x5c);
  {
    unsigned cols = r->cols + 1;
    r->cols = cols;
    char* q = p + 1;
    if (end <= q) goto fail;
    if (0x118 < cols) fog_fatal(0x67);
    r->data = q;
    char* start = q;
    for (;;) {
      if (q < end) {
        char c = *q++;
        if (c == '\t') { q[-1] = 0; continue; }
        if (c != '\r') continue;
        q[-1] = 0;
        if (*q != '\n') fog_fatal(0x75);
        q = q - 1 + 2;
        r->rows++;
      }
      if (Ordinal_508(start, "Expansion", 10) == 0) {
        r->rows--;
        Ordinal_491(start, q, end - q);
        end += start - q;
        q = start;
      }
      start = q;
      if (end <= q) return r;
    }
  }
fail:
  void* t = r->base; FUN_6ff6b0c0(&t);
  t = r; FUN_6ff6b0c0(&t);
  return nullptr;
}

// 1.13c Fog+0x12930
int Fog_12930(ExcelIdx* t, uint32_t key) {
  if (!t) fog_fatal(0x3d1);
  if (t->flag) fog_fatal(0x3d8);
  if (t->cap <= t->count) {
    int ncap = t->cap + 0x40;
    t->ents = (uint32_t(*)[2])FUN_6ff6cda0(0, t->ents, ncap * 8, "..\\Source\\Fog\\Src\\Excel\\Excel.cpp", 0x3df);
    memset(t->ents + t->cap, 0, 0x200);
    t->cap = ncap;
  }
  int n = t->count;
  for (;; key++) {
    int lo = 0, hi = n;
    bool dup = false;
    while (lo < hi) {
      int mid = (hi - lo) / 2 + lo;
      uint32_t v = t->ents[mid][0];
      if (key <= v) { hi = mid; if (key >= v) { dup = true; break; } }
      else lo = mid + 1;
    }
    if (dup) continue;
    if (lo < n) memmove(&t->ents[lo + 1], &t->ents[lo], (n - lo) * 8);
    t->ents[lo][0] = key;
    t->ents[lo][1] = t->count;
    int ret = t->ents[lo][1];
    t->count++;
    return ret;
  }
}

// 1.13c Fog+0x12610
void Fog_12610(int* t, const void* name) {
  if (!t) fog_fatal(0x4b2);
  if (t[2]) fog_fatal(0x4b6);
  uint8_t* rec = (uint8_t*)FUN_6ff6b560(nullptr);
  if (!rec) FUN_6ff60f30(2, "..\\Source\\Fog\\Src\\Excel\\Excel.cpp", 0x4b9, DAT_6ff768ec);
  memset(rec, 0, 0x2c);
  Ordinal_501(rec, name, 0x20);
  for (uint8_t* p = rec; *p && p < rec + 0x20; p++) *p = DAT_6ff76690[(char)*p];
  *(int*)(rec + 0x20) = *t;
  FUN_6ff62020(rec);
  (*t)++;
}

// 1.13c Fog+0x1F550
void Fog_1F550(void) {
  memset(DAT_6ff80ed0, 0, sizeof(DAT_6ff80ed0));  // 2 zeroed words propagated over 0x1fe copies
  memset(DAT_6ff9af48, 0, sizeof(DAT_6ff9af48));
  FUN_6ff6efd0(DAT_6ff7f210);
}

// 1.13c Fog+0x1EDA0
int Fog_1EDA0(uint8_t* out, int outCap, const uint8_t* in, int inLen) {
  uint32_t acc = 0;
  int bits = 0x20;
  uint8_t* o = out;
  for (;;) {
    for (; bits > 7 && inLen; inLen--) { bits -= 8; acc |= (uint32_t)*in++ << (bits & 0x1f); }
    const uint8_t* e = DAT_6ff806d0[acc >> 24];
    uint8_t k = e[0];
    uint8_t sym = e[((acc >> ((0x18 - k) & 0x1f)) & DAT_6ff7f310[k]) + 1];
    uint8_t len = DAT_6ff7f210[sym];
    bits += len;
    if (bits > 0x20) return (int)(o - out);
    if (outCap == 0) return 0;
    *o++ = sym;
    acc <<= (len & 0x1f);
    outCap--;
  }
}
