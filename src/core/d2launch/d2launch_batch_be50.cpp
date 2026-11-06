// D2Launch batch be50: BE50 BEB0 C3F0 C430 CB00 (CharSel/realm list). Register-passed args shown as explicit params.
#include <cstdint>
#include <cstddef>
#include <cstring>

__attribute__((weak)) extern void* Ordinal_10042(int32_t size, int32_t);
__attribute__((weak)) extern int32_t Ordinal_10154(int32_t, int32_t, void*, void*);
__attribute__((weak)) extern void Ordinal_10080(int32_t, int32_t);
__attribute__((weak)) extern void Ordinal_10172(int32_t, int32_t);
__attribute__((weak)) extern void Ordinal_10093(void);
__attribute__((weak)) extern int32_t Ordinal_10003(void);
__attribute__((weak)) extern int32_t Ordinal_10227(void);
int32_t SStrCmpI(const char* a, const char* b, size_t n);  // Storm #509 (src/core/d2moo/compat/stubs.cpp)
__attribute__((weak)) extern int32_t Ordinal_422(const char*, const char*, int32_t, void*, int32_t);
__attribute__((weak)) extern void Ordinal_425(const char*, const char*, int32_t, void*);
__attribute__((weak)) extern void Ordinal_501(void*, const char*, int32_t);
__attribute__((weak)) extern void Ordinal_10024(const char*, void*);
__attribute__((weak)) extern void* Ordinal_10265(int32_t id);
__attribute__((weak)) extern void Ordinal_10018(void);
__attribute__((weak)) extern int Ordinal_10069(unsigned);
__attribute__((weak)) extern void Ordinal_10201(void);
__attribute__((weak)) extern void Ordinal_10000(void);
__attribute__((weak)) extern void OutputDebugStringA(const char*);
__attribute__((weak)) extern void _exit(int);
__attribute__((weak)) extern void Unicode_toUtf(void* dst, const void* src, int32_t n);
__attribute__((weak)) extern void Unicode_strcpy(void* dst, const void* src);
__attribute__((weak)) extern const char* BNGatewayAccess_Realm(int32_t);
__attribute__((weak)) extern int32_t FUN_6fa52b10(void);
__attribute__((weak)) extern void FUN_6fa55b60(void);
__attribute__((weak)) extern void FUN_6fa55090(int32_t, void*, int32_t);
__attribute__((weak)) extern void FUN_6fa57df0(void);
__attribute__((weak)) extern void FUN_6fa4e250(void);
__attribute__((weak)) extern void FUN_6fa4b600(void);
__attribute__((weak)) extern void FUN_6fa4a8c0(void);
__attribute__((weak)) extern void FUN_6fa4a230(const void*, int32_t);
__attribute__((weak)) extern void FUN_6fa4bdf0(void);
__attribute__((weak)) extern int32_t FUN_6fa4c720(const char*, const char*, int32_t);
__attribute__((weak)) extern int32_t FUN_6fa4b5b0(void);
__attribute__((weak)) extern int32_t DAT_6fa65e78, DAT_6fa65ad4, DAT_6fa65ed0, DAT_6fa64db4, DAT_6fa64db8, DAT_6fa65eb8, DAT_6fa65eb0, DAT_6fa65edc, DAT_6fa65ee0, DAT_6fa65ab4, DAT_6fa65aa8, DAT_6fa65ee4;
__attribute__((weak)) extern uint8_t DAT_6fa65758[];
__attribute__((weak)) extern uint32_t* DAT_6fa65e74;   // 32-bit slot words
__attribute__((weak)) extern uint8_t* DAT_6fa65abc;
__attribute__((weak)) extern uint8_t* DAT_6fa656c4;
__attribute__((weak)) extern uint8_t* DAT_6fa65ec8;

// 1.13c D2Launch+0xBE50  (name in EDI; each table entry's first dword is a string pointer)
int32_t d2l_be50_FUN_6fa4be50(const char* name) {
  for (int32_t i = 0;; i++) {
    int32_t v = 0;
    if (i < DAT_6fa65e78) {
      int32_t* p = (int32_t*)(uintptr_t)DAT_6fa65e74[i];
      if (!p) { Ordinal_10024("", Ordinal_10265(0x1143)); _exit(-1); }
      v = *p;
    }
    if (!v) return 0;
    if (SStrCmpI(name, (const char*)(intptr_t)v, 0x7fffffff) == 0) return 1;
  }
}

// 1.13c D2Launch+0xBEB0 (stdcall, ret 0x10: name, src, user, kind). Retail pointers are 32-bit words; callees and the list head/count are seams.
struct D2lBeb0S {
  uint32_t (*alloc)(uint32_t size, uint32_t line);          // 0x97DE
  void (*toUtf)(uint32_t dst, uint32_t src, uint32_t n);    // D2Lang Unicode::toUtf
  void (*ustrcpy)(uint32_t dst, uint32_t src);              // D2Lang Unicode::strcpy
  uint32_t (*f99ca)(uint32_t kind, uint32_t five, uint32_t p0, uint32_t p1);
  void (*f9a0c)(uint32_t h, uint32_t z);
  void (*f9a06)(uint32_t h, uint32_t z);
  void (*f9a2a)(uint32_t h, uint32_t five);
  void (*fb600)(void);
};
static inline uint8_t* d2lP(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
static inline uint32_t d2lRd(uint32_t a) { uint32_t v; memcpy(&v, d2lP(a), 4); return v; }
static inline void d2lWr(uint32_t a, uint32_t v) { memcpy(d2lP(a), &v, 4); }
extern "C" int32_t d2l_beb0_core(uint32_t name, uint32_t src, uint32_t user, uint32_t kind, uint32_t* head, uint32_t* count, const D2lBeb0S* s) {
  const uint32_t n = s->alloc(0x350, 0xbc4);
  s->toUtf(n, name, 0x100);
  s->ustrcpy(n + 0x100, src);
  uint8_t* b = d2lP(n);
  memset(b + 0x300, 0, 0x10);
  memset(b + 0x310, 0, 0x10);
  b[0x320] = (uint8_t)kind;
  memset(b + 0x322, 0, 8);
  memset(b + 0x32e, 0, 3);
  d2lWr(n + 0x338, 0);
  d2lWr(n + 0x34c, 0);
  memset(b + 0x310, 0xff, 0x10);
  b[0x301] = 1;
  const uint32_t h = s->f99ca(b[0x320], 5, n + 0x300, n + 0x310);
  d2lWr(n + 0x338, h);
  d2lWr(n + 0x334, user);
  s->f9a0c(h, 0);
  s->f9a06(d2lRd(n + 0x338), 0);
  s->f9a2a(d2lRd(n + 0x338), 5);
  if (*head == 0) {
    *head = n;
  } else {
    uint32_t t = *head;
    while (d2lRd(t + 0x34c)) t = d2lRd(t + 0x34c);
    d2lWr(t + 0x34c, n);
  }
  (*count)++;
  s->fb600();
  return 1;
}

// legacy host-width wrapper kept for the CB00 port (retail layout is d2l_beb0_core)
int32_t d2l_beb0_FUN_6fa4beb0(const void* name, const void* src, char kind, int32_t user) {
  uint8_t* n = (uint8_t*)Ordinal_10042(0xbc4, 0);
  Unicode_toUtf(n, name, 0x100);
  Unicode_strcpy(n + 0x100, src);
  memset(n + 0x300, 0, 0x10);
  memset(n + 0x310, 0xff, 0x10);   // 0x300..0x30f zero, 0x310..0x31f = -1
  n[0x300] = 0;
  n[0x320] = (uint8_t)kind;
  memset(n + 0x322, 0, 8);
  memset(n + 0x32e, 0, 3);
  memset(n + 0x338, 0, 4);
  memset(n + 0x34c, 0, 4);
  n[0x301] = 1;
  int32_t h = Ordinal_10154(kind, 5, n + 0x300, n + 0x310);
  *(int32_t*)(n + 0x338) = h;
  *(int32_t*)(n + 0x334) = user;
  Ordinal_10080(h, 0);
  Ordinal_10172(*(int32_t*)(n + 0x338), 0);
  Ordinal_10093();
  if (DAT_6fa65ec8) {   // append to tail of the list linked through +0x34c
    uint8_t* t = DAT_6fa65ec8;
    while (*(uint8_t**)(t + 0x34c)) t = *(uint8_t**)(t + 0x34c);
    *(uint8_t**)(t + 0x34c) = n;
    n = DAT_6fa65ec8;
  }
  DAT_6fa65ec8 = n;
  DAT_6fa65ed0++;
  FUN_6fa4b600();
  return 1;
}

struct D2lC3f0S {
  uint32_t (*lang)(uint32_t id);                                  // 0x9B32 (ECX = string id, EAX = the string)
  void (*f_beb0)(uint32_t a, uint32_t b, uint32_t one, uint32_t c); // 0xBEB0 (4 stack words, ret 0x10)
};
// 1.13c D2Launch+0xC3F0  (g25ad4, g25ed0 pinned; g24db4 written)
extern "C" void d2l_c3f0_core(int32_t g25ad4, int32_t g25ed0, int32_t* g24db4, const D2lC3f0S* s) {
  uint32_t a = s->lang(0x1419);
  uint32_t b = s->lang(g25ad4 == 1 ? 0x141b : 0x141a);
  *g24db4 = g25ed0;
  s->f_beb0(a, b, 1, 0xc);
}

// 1.13c D2Launch+0xC430
int32_t d2l_c430_FUN_6fa4c430(void) {
  DAT_6fa65eb8 = 0;
  if (DAT_6fa65ad4 != 1) return 1;
  FUN_6fa55b60();
  uint8_t* b = DAT_6fa65abc;
  if (DAT_6fa65e78 == -1) {
    if (DAT_6fa656c4) {
      DAT_6fa65eb0 = 1;
      b[0x35d] = 0; b[0x37d] = 0;
      OutputDebugStringA("Disconnect from BNET\n");
      Ordinal_10018();
      (*(void (**)(void))(DAT_6fa656c4 + 0x10))();
    }
    FUN_6fa55090(Ordinal_10003(), (void*)FUN_6fa57df0, 1);
    return 0;
  }
  if (DAT_6fa65e78 == 0) {
    FUN_6fa4e250();
    FUN_6fa55090(Ordinal_10003(), (void*)FUN_6fa57df0, 1);
    return 0;
  }
  FUN_6fa52b10();
  Ordinal_10069(0);
  Ordinal_10201();
  DAT_6fa65edc = FUN_6fa52b10();
  DAT_6fa65ee0 = FUN_6fa52b10();
  FUN_6fa52b10();
  DAT_6fa65ab4 = FUN_6fa52b10();
  DAT_6fa65aa8 = FUN_6fa52b10();
  Ordinal_10003();
  FUN_6fa52b10();
  Ordinal_10000();
  if (!Ordinal_422("Diablo II", "Preferred Realm", 0, DAT_6fa65758, 0x100) || !d2l_be50_FUN_6fa4be50((const char*)DAT_6fa65758)) {
    int32_t g = (*(int32_t (**)(void))(DAT_6fa656c4 + 0x98))();
    Ordinal_501(DAT_6fa65758, BNGatewayAccess_Realm(g), 0x7fffffff);
    Ordinal_425("Diablo II", "Preferred Realm", 0, DAT_6fa65758);
    if (!d2l_be50_FUN_6fa4be50((const char*)DAT_6fa65758)) {
      Ordinal_501(DAT_6fa65758, (const char*)(intptr_t)FUN_6fa4b5b0(), 0x7fffffff);
      Ordinal_425("Diablo II", "Preferred Realm", 0, DAT_6fa65758);
    }
  }
  return 1;
}

struct D2lCb00S {
  int32_t (*f10227)(void);                                                                  // 0x980E (Fog 10227)
  uint32_t (*lang)(uint32_t id);                                                            // 0x9B32 (ECX = id)
  void (*f_beb0)(uint32_t a, uint32_t b, uint32_t kind, uint32_t user);                     // 0xBEB0 (4 stack words)
  int32_t (*f_c720)(uint32_t eax, uint32_t s1, uint32_t s2, uint32_t extra, uint32_t z0, uint32_t z1); // 0xC720 (EAX = arg0, 5 stack words)
  void (*f_bdf0)(void);                                                                     // 0xBDF0
  void (*f_a230)(uint32_t eax, const void* pkt, uint32_t len);                              // 0xA230 (EAX = arg0, ECX = pkt, 1 stack word)
  void (*f_a8c0)(void);                                                                     // 0xA8C0
};
// 1.13c D2Launch+0xCB00  stdcall(arg0, pkt, redraw), ret 0xc. g25ad4/g25ed0 pinned; g64db4, g64db8, g65ee4 written.
extern "C" void d2l_cb00_core(uint32_t arg0, const uint8_t* pkt, int32_t redraw, int32_t g25ad4, int32_t g25ed0,
                              int32_t* g64db4, int32_t* g64db8, int32_t* g65ee4, const D2lCb00S* s) {
  uint32_t left = *(const uint16_t*)(pkt + 7);
  uint8_t kind = pkt[0];
  const char* p = (const char*)pkt + 9;
  if (!s->f10227()) {
    uint32_t a = s->lang(0x1419);
    uint32_t c = s->lang(g25ad4 == 1 ? 0x141b : 0x141a);
    *g64db4 = g25ed0;
    s->f_beb0(a, c, 1, 0xc);
  }
  uint32_t cnt = *(const uint16_t*)(pkt + 3);
  if (left < cnt) cnt = left;
  for (; cnt; cnt--) {
    uint32_t extra = 0;
    const char* s1 = p;
    if (kind == 0x19) { memcpy(&extra, p, 4); p += 4; s1 = p; }
    while (*p++) {}
    const char* s2 = p;
    const char* q = p;
    while (*q++) {}
    if (!s->f_c720(arg0, (uint32_t)(uintptr_t)s1, (uint32_t)(uintptr_t)s2, extra, 0, 0)) { left--; s->f_bdf0(); redraw = 0; }
    p = q;
  }
  if (redraw) s->f_a230(arg0, pkt, (uint32_t)(p - (const char*)pkt));
  if (*(const uint16_t*)(pkt + 1) <= left) *g65ee4 = 1;
  if (*(const uint16_t*)(pkt + 7) < *(const uint16_t*)(pkt + 3)) {
    *g64db8 = g25ed0;
    uint32_t a = s->lang(0x2b3b);
    uint16_t z = 0;
    s->f_beb0(a, (uint32_t)(uintptr_t)&z, 2, 0x12);
  }
  s->f_a8c0();
}
void d2l_cb00_FUN_6fa4cb00(int32_t arg0, const uint8_t* pkt, int32_t redraw) {
  static const D2lCb00S S = {
    Ordinal_10227,
    [](uint32_t) { return (uint32_t)Ordinal_10003(); },
    [](uint32_t a, uint32_t b, uint32_t k, uint32_t u) { d2l_beb0_FUN_6fa4beb0((void*)(intptr_t)a, (void*)(intptr_t)b, (char)k, (int32_t)u); },
    [](uint32_t, uint32_t a, uint32_t b, uint32_t c, uint32_t, uint32_t) { return FUN_6fa4c720((const char*)(intptr_t)a, (const char*)(intptr_t)b, (int32_t)c); },
    FUN_6fa4bdf0,
    [](uint32_t, const void* k, uint32_t n) { FUN_6fa4a230(k, (int32_t)n); },
    FUN_6fa4a8c0 };
  d2l_cb00_core((uint32_t)arg0, pkt, redraw, DAT_6fa65ad4, DAT_6fa65ed0, &DAT_6fa64db4, &DAT_6fa64db8, &DAT_6fa65ee4, &S);
}
