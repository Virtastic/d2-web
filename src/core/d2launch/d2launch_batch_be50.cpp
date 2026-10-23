// D2Launch batch be50: BE50 BEB0 C3F0 C430 CB00 (CharSel/realm list). Register-passed args shown as explicit params.
#include <cstdint>
#include <cstring>

__attribute__((weak)) extern void* Ordinal_10042(int32_t size, int32_t);
__attribute__((weak)) extern int32_t Ordinal_10154(int32_t, int32_t, void*, void*);
__attribute__((weak)) extern void Ordinal_10080(int32_t, int32_t);
__attribute__((weak)) extern void Ordinal_10172(int32_t, int32_t);
__attribute__((weak)) extern void Ordinal_10093(void);
__attribute__((weak)) extern int32_t Ordinal_10003(void);
__attribute__((weak)) extern int32_t Ordinal_10227(void);
__attribute__((weak)) extern int32_t Ordinal_509(void);
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
__attribute__((weak)) extern int32_t** DAT_6fa65e74;
__attribute__((weak)) extern uint8_t* DAT_6fa65abc;
__attribute__((weak)) extern uint8_t* DAT_6fa656c4;
__attribute__((weak)) extern uint8_t* DAT_6fa65ec8;

// 1.13c D2Launch+0xBE50
int32_t d2l_be50_FUN_6fa4be50(void) {
  for (int32_t i = 0;; i++) {
    int32_t v = 0;
    if (i < DAT_6fa65e78) {
      int32_t* p = DAT_6fa65e74[i];
      if (!p) { Ordinal_10024("", Ordinal_10265(0x1143)); _exit(-1); }
      v = *p;
    }
    if (!v) return 0;
    if (Ordinal_509() == 0) return 1;
  }
}

// 1.13c D2Launch+0xBEB0  (name utf in EBX, wide src on stack, kind, EDI = user dword)
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

// 1.13c D2Launch+0xC3F0
void d2l_c3f0_FUN_6fa4c3f0(void) {
  int32_t a = Ordinal_10003();
  char c = (char)Ordinal_10003();
  DAT_6fa64db4 = DAT_6fa65ed0;
  d2l_beb0_FUN_6fa4beb0((void*)(intptr_t)a, nullptr, c, 0);
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
  if (!Ordinal_422("Diablo II", "Preferred Realm", 0, DAT_6fa65758, 0x100) || !d2l_be50_FUN_6fa4be50()) {
    int32_t g = (*(int32_t (**)(void))(DAT_6fa656c4 + 0x98))();
    Ordinal_501(DAT_6fa65758, BNGatewayAccess_Realm(g), 0x7fffffff);
    Ordinal_425("Diablo II", "Preferred Realm", 0, DAT_6fa65758);
    if (!d2l_be50_FUN_6fa4be50()) {
      Ordinal_501(DAT_6fa65758, (const char*)(intptr_t)FUN_6fa4b5b0(), 0x7fffffff);
      Ordinal_425("Diablo II", "Preferred Realm", 0, DAT_6fa65758);
    }
  }
  return 1;
}

// 1.13c D2Launch+0xCB00
void d2l_cb00_FUN_6fa4cb00(int32_t, const uint8_t* pkt, int32_t redraw) {
  uint32_t left = *(const uint16_t*)(pkt + 7);
  uint8_t kind = pkt[0];
  const char* p = (const char*)pkt + 9;
  if (!Ordinal_10227()) {
    int32_t a = Ordinal_10003();
    char c = (char)Ordinal_10003();
    DAT_6fa64db4 = DAT_6fa65ed0;
    d2l_beb0_FUN_6fa4beb0((void*)(intptr_t)a, nullptr, c, 0);
  }
  uint32_t cnt = *(const uint16_t*)(pkt + 3);
  if (left < cnt) cnt = left;
  for (; cnt; cnt--) {
    int32_t extra = 0;
    const char* s1 = p;
    if (kind == 0x19) { memcpy(&extra, p, 4); p += 4; s1 = p; }
    while (*p++) {}
    const char* s2 = p;
    const char* q = p;
    while (*q++) {}
    if (!FUN_6fa4c720(s1, s2, extra)) { left--; FUN_6fa4bdf0(); redraw = 0; }
    p = q;
  }
  if (redraw) FUN_6fa4a230(pkt, (int32_t)(p - (const char*)pkt));
  if (*(const uint16_t*)(pkt + 1) <= left) DAT_6fa65ee4 = 1;
  if (*(const uint16_t*)(pkt + 7) < *(const uint16_t*)(pkt + 3)) {
    DAT_6fa64db8 = DAT_6fa65ed0;
    d2l_beb0_FUN_6fa4beb0((void*)(intptr_t)Ordinal_10003(), nullptr, 8, 0);
  }
  FUN_6fa4a8c0();
}
